#!/bin/bash

cd $( dirname -- "$0" )

if [ -d "$HOME/Arduino/libraries/Buzzer" ]
then
    mkdir -p "$HOME/Arduino/.backups"
    mv "$HOME/Arduino/libraries/Buzzer" "$HOME/Arduino/.backups"
fi

mkdir -p "$HOME/Arduino/libraries"

cp -vr "./Buzzer" "$HOME/Arduino/libraries"
