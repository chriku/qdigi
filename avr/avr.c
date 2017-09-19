#define F_CPU 1000UL
/* uC: AT90S2313 */
#include <avr/io.h>
#include <avr/interrupt.h>
int value=0;
int main(void)
{
  DDRA=2;
  // Timer 0 konfigurieren
  TCCR0A = (1<<CS01); // Prescaler 8

  // Overflow Interrupt erlauben
  TIMSK |= (1<<TOIE0);

  // Global Interrupts aktivieren
  sei();

  while(1)
  {
    PORTA=value;
    /* Sonstige Aktionen */
  }
}

/*
Der Overflow Interrupt Handler
wird aufgerufen, wenn TCNT0 von
255 auf 0 wechselt (256 Schritte),
d.h. ca. alle 2 ms
*/
#ifndef TIMER0_OVF_vect
// Für ältere WinAVR Versionen z.B. WinAVR-20071221
#define TIMER0_OVF_vect TIMER0_OVF0_vect
#endif

ISR (TIMER0_OVF_vect)
{
  value=value^2;
}
