#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>


int main (void)
{
	// F_CPU is the number of operation the chip's processor is able to do in 1 sec.
	// Since we want to execute the code only every 0.5 second we need to divide it by 2.
	// But first since we need to execute our code every time.
	// We have to divide F_CPU by the amount of cycles that it needs to execute, 
	// which after looking into the dissassembly of the code takes 6 cycles
	// so we first divide F_CPU by the amount of cycles (6) and the amount of times we want
	// to do our operation par seconds, 2 for a frequency of 1Hz
	uint32_t timer_max = F_CPU / 6 / 2;
	uint32_t timer = 0;


	DDRB |= (1 << DDB1); // put DDB0 in Output mode
    while(1)
	{
		if (timer++ >= timer_max)
		{
			PORTB ^= (1 << PORTB1); //Do a XOR to reverse
			timer = 0;
		}	
	}
}