#include <avr/io.h>

void Delay(void);

int main(void)
{
    // LEDs are on pins PD0 and PD5
    DDRD |= ((1 << PD0) | (1 << PD5));

    while(1)
    {
        PORTD &= ~(1 << PD0);           // switch PD0 LED off
        PORTD |=  (1 << PD5);           // switch PD5 LED on
        Delay();
        PORTD &= ~(1 << PD5);           // switch PD5 LED off
        PORTD |=  (1 << PD0);           // switch PD0 LED on
        Delay();
    }
}

void Delay(void)
{
    volatile unsigned int del = 400;

    while(del--);
}
