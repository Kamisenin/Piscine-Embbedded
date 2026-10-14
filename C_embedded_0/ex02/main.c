#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>


void inject_bit(uint8_t source, volatile uint8_t *destination, uint8_t pos_source, uint8_t pos_dest)
{
	uint8_t bit = (source >> pos_source) & 1;

	*destination = ((*destination) & ~(1 << pos_dest)) | (bit << pos_dest);
}

int main (void)
{
	//Button SW1 is linked to PD2, since we want DDRD2 to listen to input coming from the pull-up resistor and
	//send current to the DDB0, PB0. Since all of the pins are set to input on base, we will not change DDRD2 state

	DDRB |= (1 << DDB0); // We set the pin of B0 to accept current
	// we keep PORTB in the loop to listen for button beng pressed, if it were not in the loop, it would compute only once at boot
	while(1)
	{
		// PIN allows us to check current from a pin
		inject_bit(~PIND, &PORTB, PIND2, PORTB0); // here since the board pull up resistor is inversed (no  current when pressing), we reverse PIND for our bit to allow current coming to PIND2 
	}
}