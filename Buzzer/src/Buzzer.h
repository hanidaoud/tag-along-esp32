#include <Arduino.h>

#ifndef Buzzer_h

#define Buzzer_h
#define BUZZER_ACTIVE 2300
#define BUZZER_ERROR 100
#define BUZZER_TIMEOUT 1000

class Buzzer
{
    int pin;

    public:

    void ok(int);
    void error(int, int);
    void passive(int, int);
    Buzzer(int __pin)
    {
        pin = __pin;
    }
};

#endif
