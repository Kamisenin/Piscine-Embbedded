#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#include "interrupt.h"
#include "timer.h"
#include "uart.h"

int main(void)
{
	CLEAR_INTERRUPT;
	uart_init();
	PRR &= ~_BV(PRADC); // disable porwer reduction for ADC (Manual recommend always putting it to 0 for security)
	ADMUX = _BV(REFS0) | _BV(REFS1) | _BV(MUX3); // Set to ADC8
	// Enable (ADEN), set prescaler to 128 (ADPSx), set interruption (ADIE) set auto trigger (ADATE)
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);   
	ADCSRB = _BV(ADTS2) | _BV(ADTS0); // use Timer 1
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
	// We subtract the offset to remove the sensor's base voltage at zero degrees,
	// isolating only the portion of the reading that changes with temperature.
	// divide by 1.1 a per our reference, to transform our obtained ADC count to celsius
	uart_putnbr_base((ADC - 368) / 1.1, 10);
	uart_printstr("\r\n");
}