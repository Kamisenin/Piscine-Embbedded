#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>



int main (void)
{
	DDRB |= (1 << DDB1); // put Data Direction of B1 to output mode

	// when looking at the datasheet we see that de B1 Led is linked to TCCR1A throught OC1A.
	// So we check the documentation table and see that to toggle this led we have to
	// activate the first bit of COM1A
	TCCR1A |= (1 << COM1A0);
	// WGM12 correspond to the clock mode of Timer1, here we want CTC (which stops the clock and put it back to 0 at the desired number)
	TCCR1B |= (1<< WGM12);
	// OCR1A is the variable of Timer1's CTC mode set the clock to 0 
	OCR1A = 31250; // OCR1A = ((F_CPU * Time_in_seconds) / prescaler) - 1

	// to selection our prescaler we have to use the method previously stated an check that it doesn't go past 65535, 
	// the max time of Timer1. Here we have selected 256 which when looking at the doc corresponds to CS12
	TCCR1B |= (1 << CS12);
    
	while(1){}
}