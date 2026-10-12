#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>


/*
 * For this exercice we need cyclical notifications. 
 * to do so when looking at the documentation we see a ping MODE in the Timer1
 * that allows us to ping and light the LED until a certain value is reached
 * this is called the PWM mode, there are 3 different modes :
 * FAST PWM that count from BOTTOM to TOP and set itself at BOTTOM once top is reached
 * PWM Phase Correct : it counts to TOP and then counts back to BOTTOM 
 * PWM Phase & Frequency Correct : 
 * * does the same as the previous one at the difference that it
 * * actualise OCRNx value once it reaches the BOTTOM of the clock instead of the TOP
 * * which ensures that the BOTTOM and TOP count will still do the same length even if you change TOP
*/
int main (void)
{
	DDRB |= (1 << DDB1); // put Data Direction of B1 to output mode

	// activate the first bit of COM1A and activate fast PWM mode (MODE 13 in the Documentation)
	TCCR1A |= (1 << COM1A1) | (1 << WGM11); //  | (1 << COM1A0) 
	TCCR1B |= (1<< WGM12) | (1 << WGM13);
	// For PWM, ICR1 is the TOP variable, there is another version of PWM that uses OCR1A as TOP
	// we need to go with this mode and use ICR1 as the TOP variable and OCR1A as Cycle variable.
	// also, since we want a frequency of 1hz and that we dont have to UP th LED and down the LED we double our TOP to reach a frequency of 1Hz
	ICR1 = 62499; // ICR1 = ((F_CPU * Time_in_seconds) / prescaler) - 1
	// OCR1A = 6250; // 10% of ICR1, which is 31250 / 10
	OCR1A = 55000;

	TCCR1B |= (1 << CS12); // select prescaler to start the timer
    
	while(1){}
}