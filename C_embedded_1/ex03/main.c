#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define SW1 PD2
#define SW2 PD4
#define BUTTON_STATE (~PIND & ((1 << SW1) | (1 << SW2)))

#define TOP 62499
#define PERCENT 6250

static inline void increaseDuty10Pct(void)
{
    if (OCR1A <= TOP - PERCENT)
        OCR1A += PERCENT;
    else
        OCR1A = ICR1;
}

static inline void decreaseDuty10Pct(void)
{
    if (OCR1A > PERCENT)
        OCR1A -= PERCENT;
    else
        OCR1A = PERCENT;
}

int main (void)
{

	uint8_t old_state = BUTTON_STATE;

	DDRB |= (1 << DDB1); // put Data Direction of B1 to output mode

	// activate the first bit of COM1A and activate fast PWM mode (MODE 13 in the Documentation)
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);
	TCCR1B |= (1<< WGM12) | (1 << WGM13);
	ICR1 = TOP; // ICR1 = ((F_CPU * Time_in_seconds) / prescaler) - 1
	OCR1A = PERCENT; // Preset of ICR1, at 10%

	TCCR1B |= (1 << CS12); // select prescaler to start the timer

	PORTD |= (1 << PD2) | (1 << PD4);
    
	while(1)
	{
		
		if (old_state != BUTTON_STATE) 
		{
			_delay_ms(20);
			uint8_t new_state = BUTTON_STATE & ~old_state;

			if (new_state & (1 << SW1))
				increaseDuty10Pct();
			if (new_state & (1 << SW2))
				decreaseDuty10Pct();
			old_state = new_state;
		}
	}
}


old 	= 0b00
current = 0b01
