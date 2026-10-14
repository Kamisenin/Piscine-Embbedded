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
	// ADMU = ADC (Analog-to-digital Converter) Multiplexer Selection Register
	// here as subject requires we set REFS0 bit to use AVcc as Voltage Reference (24-3 p.257)
	// Since there is 1 ADC but multiple PIN output for each Analog captors, we have to set the right PIN we want to use
	// Here we want to use ADC0 since its the one linked to RV1
	// in this exercice we also wan only the 8 first bits of value so we switch to Left adjust (ADLAR) to be able to only look at ADCH 
	ADMUX = _BV(REFS0) | _BV(ADLAR); //  ADC0 is linked to 0000 Mux adress, so we don't need to set any mux, so we use an eq to ensute that every other option is set to 0
	/*
	 * ADCSRx = ADC Control and Status Register. It changes ADC behavior depending on the bit that are set
	 * At start DC is not enabled for power comsuption reason so we set the Enable bit with ADEN (ADC enable)
	 * For those same reason, when we want to get ADC values we have set ADSC bit (ADC start conversion) 
	 * and wait for conversion to be done either by using interruption by setting ADIE bit (ADC interrupt Enable)
	 */ 
	// Enable (ADEN) and set prescaler (ADPSx) to adjust resolution to max as zell a Setting up interruption (ADIE) and Auto trigger (ADATE)
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// Since we want to get ADC value every 20ms we wil set the source to our hardware timer
	// ADTS (ADC Auto Trigger Source) is divided in 3 bits which define the Comparator source that will trigger the conversion   
	ADCSRB = _BV(ADTS2) | _BV(ADTS0); // We will use Timer1 since Timer0 and 2 are too short for a 20ms wait
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
	uart_putnbr_base(ADCH, 16);
	uart_printstr("\r\n");
}