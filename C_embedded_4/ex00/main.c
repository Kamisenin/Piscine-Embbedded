#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "interrupt.h"
#include "bit_operation.h"
#include <util/delay.h>

volatile uint8_t expected_edge = 0;

int main()
{
	CLEAR_INTERRUPT;
	DDRD &= ~_BV(DDD2);
	// SET_BIT(PORTD, PORTD2);
	SET_BIT(DDRB, DDB0);
	// EICRA = External Interrupt Control Register A (13.2.1 p.80)
	EICRA |= _BV(ISC01); // activate interrupt on falling edge (Push button) (13-2 p.80)
	EIMSK |= _BV(INT0); // activate external interrupt
	SET_INTERRUPT;
	while (1);
}

void	INT0_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	INT0_vect(void)
{
	if (expected_edge == 0)
    {
		PORTB ^= _BV(PORTB0);
        _delay_ms(50);
        EICRA |= _BV(ISC00) | _BV(ISC01); // activate interrupt on rising edge (Release button) (13-2 p.80)
        expected_edge = 1;
    }
    else
    {
		_delay_ms(50);
        EICRA = (EICRA & ~_BV(ISC00)) | _BV(ISC01);
        expected_edge = 0;
    }
	// EIFR = External Interrupt Flag Register (13.2.3 p.81)
	EIFR |= _BV(INTF0); // set interrupt flag 0 to clear it and cancel other bounce interrupts (13.2.3 p.81)
}