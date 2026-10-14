#ifndef F_CPU
 #define F_CPU 16000000UL
#endif
#include <avr/io.h>


int main (void)
{
	// DDRX is meant to control wether the DDR (Data Direction Register) of X will input or output current
	DDRB |= (1 << DDB0); // We set the pin of B0 to accept current 
        // PORTX, if DDR is in output, allows to manage if we want to block or let the current pass
	PORTB |= (1 << PB0); // controls the resistance
}
