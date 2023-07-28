#include "Buzzer.h"

void Buzzer::ok(int duration)
{
    tone(Buzzer::pin, BUZZER_ACTIVE);
    delay(duration);
    noTone(Buzzer::pin);
}

void Buzzer::error(int duration, int pseudoFrequency)
{
    int period = ( int ) (500/pseudoFrequency);
    for (int i = 0; i < pseudoFrequency * duration / 1000; i++)
    {
        tone(Buzzer::pin, BUZZER_ACTIVE);
        delay(period);
        noTone(Buzzer::pin);
        delay(period);
    }
}

void Buzzer::passive(int duration, int frequency)
{
    tone(Buzzer::pin, frequency);
    delay(duration);
    noTone(Buzzer::pin);
}
