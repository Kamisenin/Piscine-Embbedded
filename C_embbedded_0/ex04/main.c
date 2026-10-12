#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "bit_utils.h"

uint8_t value = 0;

void increment_val()
{
	if (value == 15)
	{
		value = 0;
		return ;
	}
	value++;
}

void decrement_val()
{
	if (value == 0)
	{
		value = 15;
		return ;
	}
	value--;
}

void display_value()
{
	inject_bit(value, &PORTB, 0, PORTB0);
	inject_bit(value, &PORTB, 1, PORTB1);
	inject_bit(value, &PORTB, 2, PORTB2);
	inject_bit(value, &PORTB, 3, PORTB4);
}

int main(void)
{
	uint8_t state_ws1 = is_button_pressed(PIND, PIND2);
	uint8_t state_ws2 = is_button_pressed(PIND, PIND4); // sw2 pin is PIND4


	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4); // we put the DDB's entry of the leds to output mode
	while(1)
	{
		if (!state_ws1)
		{
			if (is_button_pressed(PIND, PIND2))
			{
				increment_val();
				display_value();
			}
		}
		if (!state_ws2)
		{
			if (is_button_pressed(PIND, PIND4))
			{
				decrement_val();
				display_value();
			}
		}
		state_ws1 = is_button_pressed(PIND, PIND2);
		state_ws2 = is_button_pressed(PIND, PIND4);
		_delay_ms(20);
	}
}
