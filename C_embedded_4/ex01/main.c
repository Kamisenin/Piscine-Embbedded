#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "interrupt.h"
#include "bit_operation.h"
#include <util/delay.h>



void init_timers()
{
	/*INIT TIMER 1*/
	SET_BIT(TCCR1A, WGM10); // PWM phase correct in 8bit because we dont need more time (mode 1)
	SET_BIT(TCCR1A, COM1A1); // Set to activate PB1 LED under OCR1A
	OCR1A = 0;

	/*INIT TIMER 0*/
	SET_BIT(TCCR0A, WGM01); // Mode 2, CTC
	OCR0A = 30; // (F_CPU * 0.00196) / 1024 - 1 |||| ((F_CPU * Time_in_seconds) / prescaler) - 1 
	SET_BIT(TIMSK0, OCIE0A); // Interrupt Flag to interrupt when TIMER0 reachs OCR0A (OCIE0A = Outpout compare Interrupt Enable)

	/*POWER REGISTER BIT CLEAR*/
	CLEAR_BIT(PRR, PRTIM0);
	CLEAR_BIT(PRR, PRTIM1);

	/*PRESCALERS*/
	SET_BIT(TCCR1B, CS10); // 1 
	TCCR0B |= _BV(CS00) | _BV(CS02); // 1024 
}

int main()
{
	CLEAR_INTERRUPT;
	SET_BIT(DDRB, DDB1);
	init_timers();
	SET_INTERRUPT;
	while (1);
}

void	TIMER0_COMPA_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	TIMER0_COMPA_vect(void)
{
	static int8_t direction = 1;
	
	OCR1A += direction;

	if (OCR1A == 255)
		direction = -1;
	else if (OCR1A == 0)
		direction = 1;
}