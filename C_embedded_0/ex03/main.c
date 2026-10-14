#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

bool is_button_pressed()
{
	// check if the button is being pressed with pin, since the trigger is reversed, we reverse the return
	return (!(PIND & (1 << PIND2))); 
}

int main (void)
{	
	
	DDRB |= (1 << DDB0);
	while(1)
	{
		if (is_button_pressed())
			PORTB ^= (1 << PB0); //Do a XOR op to reverse the value of B0
		while (is_button_pressed()) {}
		_delay_ms(20);
	}
}
