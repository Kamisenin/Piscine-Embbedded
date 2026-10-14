#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include "interrupt.h"
#include "timer.h"
#include "rgb.h"

int main(void)
{
	CLEAR_INTERRUPT;
	init_rgb();
	set_luminosity(25);
	DDRB |= _BV(DDB0) | _BV(DDB1) | _BV(DDB2) | _BV(DDB4);
	PRR &= ~_BV(PRADC); // disable porwer reduction for ADC (Manual recommend always putting it to 0 for security)
	ADMUX = _BV(REFS0) | _BV(ADLAR); // stay at Mux0 to use RV1
	// Enable (ADEN) and set prescaler (ADPSx) to adjust resolution to max as zell a Setting up interruption (ADIE) and Auto trigger (ADATE)
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	ADCSRB = _BV(ADTS2) | _BV(ADTS0); // ADTS2 + ADTS0 = Timer1_compb
	OCR1A = 5000;
	OCR1B = 5000; // every 20ms
	TIMSK1 = _BV(OCIE1B); 
	TCCR1B |= PRESCALE_64 | CTC_1;
	SET_INTERRUPT;
	while (1);
}

void	ADC_vect(void) __attribute__((NO_INTERRUP, USED, VISIBLE));

void	ADC_vect(void)
{
	/*
	 * We divide ADC by 1.1 as we know that 1 ADC ~= 1.1V (since we used it as reference). 
	 * And Offset ADC by 368 s you can't have a 0V tension so the captor produce a base tension of
	 * around 300 to 400mV. since its different for every choip (mainly cause of the variation in silicium engraving)
	 */
	uint16_t rv1 = ADCH;
	uint16_t p = rv1 * 100UL / 255;
	PORTB = ((p >= 25) ? _BV(PB0) : 0) | ((p >= 50) ? _BV(PB1) : 0) 
		| ((p >= 75) ? _BV(PB2) : 0) | ((p == 100) ? _BV(PB4) : 0);
	wheel(rv1);
}