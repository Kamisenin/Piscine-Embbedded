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
	/*
	 * ADCSRx = ADC Control and Status Register. It changes ADC behavior depending on the bit that are set
	 * At start DC is not enabled for power comsuption reason so we set the Enable bit with ADEN (ADC enable)
	 * For those same reason, when we want to get ADC values we have set ADSC bit (ADC start conversion) 
	 * and wait for conversion to be done either by using interruption by setting ADIE bit (ADC interrupt Enable)
	 * Here since we want to successively check we every 20ms we will not use ADC interrupt and prefer to wait for ADSC
	 * to be set to 0 whcihc signifes that the conversion is done 
	 */ 
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Enable (ADEN) andset prescaler (ADPSx) to adjust resolution to max
	// Since we want to get ADC value every 20ms we wil set the source to our hardware timer
	OCR1A = 5000;
	TIMSK1 = _BV(OCIE1A); // Trigger an interrupt every 20ms 
	TCCR1B |= PRESCALE_64 | CTC_1;
	SET_INTERRUPT;
	while (1);
}

void	adc_convert()
{
	ADCSRA |= _BV(ADSC); // start conversion (ADSC = ADC Start Conversion)
	while (ADCSRA & _BV(ADSC)); //wait for conversion to be done (ADSC = 0)
	uart_putnbr_base(ADCH, 16);
}

void	TIMER1_COMPA_vect(void) __attribute__((NO_INTERRUP, USED, VISIBLE));

void	TIMER1_COMPA_vect(void)
{
	ADMUX = _BV(REFS0) | _BV(ADLAR); // set back all ADMUX bit to 0 except REF to keep reference and use ADC0 (RV1)
	adc_convert();
	uart_printstr(", ");
	ADMUX |= _BV(MUX0); // ADC1 (R14)
	adc_convert();
	uart_printstr(", ");
	ADMUX = _BV(REFS0) | _BV(MUX1); // ADC2 (R20)
	adc_convert();
	uart_printstr("\r\n");
}