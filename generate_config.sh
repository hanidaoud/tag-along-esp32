#!/usr/bin/bash

PINS=("SS_PIN" "RST_PIN" "BUZZ" "LED_WiR" "LED_WiG" "LED_OK" "LED_NOK")
DEFAULT_PINS=(5 22 14 33 32 25 26)
NET=("ESSID" "PASSWD" "IP_ADDRESS")

set_passwd()
{
    unset password

    while IFS= read -p "$prompt" -r -s -n 1 char
    do
        # Enter - accept password
        if [[ $char == $'\0' ]] ; then
            break
        fi
        # Backspace
        if [[ $char == $'\177' ]] && [[ ${#password} > 0 ]] ; then
            prompt=$'\b \b'
            password="${password%?}"
        elif [[ $char == $'\177' ]] && [[ ${#password} == 0 ]] ; then
            prompt=$''
        else
            prompt='*'
            password+="$char"
        fi
    done

}

if [ -f config.h ]
then
    read -p ":: $(tput bold)Remove existing config file [y/N] $(tput sgr0)" yn
    case $yn in
        [Yy]* ) rm config.h;;
        * ) exit 1;;
    esac
fi


read -p ":: $(tput bold)Use the default pinout [Y/n] $(tput sgr0)" yn

case $yn in
    [Nn]* ) for i in {0..6}
            do
                read -p ":: $(tput bold)Enter ${PINS[$i]}: $(tput sgr0)" p
                echo -e "#ifndef ${PINS[$i]}\n#define ${PINS[$i]} $p\n#endif\n" >> config.h
            done;;
    * ) for i in {0..6}
        do
            echo -e "#ifndef ${PINS[$i]}\n#define ${PINS[$i]} ${DEFAULT_PINS[$i]}\n#endif\n" >> config.h
        done;;
esac


for i in {0..2}
do
    if [[ $i -eq 1 ]]
    then
        echo -ne ":: $(tput bold)Enter password: $(tput sgr0)"
        set_passwd
        echo -e "#ifndef ${NET[$i]}\n#define ${NET[$i]} \"$password\"\n#endif\n" >> config.h
        echo
    else
        read -p ":: $(tput bold)Enter ${NET[$i]}: $(tput sgr0)" n
        echo -e "#ifndef ${NET[$i]}\n#define ${NET[$i]} \"$n\"\n#endif\n" >> config.h
    fi
done


read -p ":: $(tput bold)Do you want to use the default UUID [Y/n] $(tput sgr0)" yn

case $yn in
    [Nn]* ) read -p ":: $(tput bold)Enter new UUID: $(tput sgr0)" u
            echo -e "#ifndef UUID\n#define UUID \"$u\"\n#endif\n" >> config.h;;
    * ) echo -e "#ifndef UUID\n#define UUID \"2c56d4c7-5c46-4191-bd65-5dd820b41c30\"\n#endif\n" >> config.h;;
esac
