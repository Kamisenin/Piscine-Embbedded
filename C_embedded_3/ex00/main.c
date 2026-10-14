#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define NO_INTERRUP __signal__
#define USED __used__
#define VISIBLE __externally_visible__
#define I 7
/*
 * Interrupts are functions defined in a specific space of the hardware, they are space reserved that can be redefined but never moved
 * Depending on the Interrupt Vector used it will be called for different events. In our specific case, we want the interruption to be
 * called every time timer1 reaches OCR1A, so we use TIMER1_COMPA that we found at page 66 of the chip's documentation
*/
int main (void)
{
	SREG |= ~_BV(I); // disable I (Global Interrupt) inside the SREG (AVR Status Register), equivalent of cli()
	DDRD |= _BV(DDD3) | _BV(DDD5) | _BV(DDD6); // put Data Direction of D3, D5 and D, the r, g and b led to output mode

	TCCR1B |= (1<< WGM12); // CTC mode
	OCR1A = 62499; // OCR1A = ((F_CPU * Time_in_seconds) / prescaler) - 1
	
	// to activate interruptions triggering we need enable the corresponding mask
	// here we used OCR1A so we have used the COMPA (Compare Match A) vector so will have to
	// enable the OCIE1A Output Compare A Match Interrupt
	TIMSK1 |= _BV(OCIE1A);
	
	PORTD |= _BV(PORTD5);
	TCCR1B |= _BV(CS12); // select prescaler to start the timer
	SREG |= _BV(I); // enable interruptions, equivalent to sei();

	while(1){}
}


void TIMER1_COMPA_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void TIMER1_COMPA_vect(void)
{
	if (PIND & _BV(PIND3))
	{
		PORTD ^= _BV(PORTD3);
		PORTD ^= _BV(PORTD5);
	}
	else if (PIND & _BV(PIND5))
	{
		PORTD ^= _BV(PORTD5);
		PORTD ^= _BV(PORTD6);
	}
	else if (PIND & _BV(PIND6))
	{
		PORTD ^= _BV(PORTD6);
		PORTD ^= _BV(PORTD3);
	}
} 