#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SET_BIT(REGISTER, bit) (REGISTER |= _BV(bit))
#define CLEAR_LED PORTD &= ~(_BV(PORTD3) | _BV(PORTD5) | _BV(PORTD6))
#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW _BV(RED) | _BV(GREEN)
#define CYAN _BV(GREEN) | _BV(BLUE)
#define PINK _BV(RED) | _BV(BLUE)
#define WHITE _BV(RED) | _BV(GREEN) | _BV(BLUE)

#define I 7

#define NO_INTERRUP __signal__
#define USED __used__
#define VISIBLE __externally_visible__


void	display_color(uint8_t rgb)
{
	CLEAR_LED;
	if (rgb & _BV(RED))
		SET_BIT(PORTD, PORTD5);
	if (rgb & _BV(GREEN))
		SET_BIT(PORTD, PORTD6);
	if (rgb & _BV(BLUE))
		SET_BIT(PORTD, PORTD3);
}

void	setup(void)
{
	DDRD |= _BV(DDD3) | _BV(DDD5) | _BV(DDD6); // put Data Direction of D3, D5 and D, the r, g and b led to output mode

	SET_BIT(TCCR1B, WGM12); // CTC mode
	SET_BIT(TIMSK1, OCIE1A); // enable the OCIE1A Output Compare A Match Interrupt
	OCR1A = 62499; // OCR1A = ((F_CPU * Time_in_seconds) / prescaler) - 1
	SET_BIT(TCCR1B, CS12); // select prescaler to start the timer (prescaler of 256)
}

int	main(void)
{
	SREG &= ~_BV(I); // cancel interruptions
	setup();
	SREG |= _BV(I); // activate interruptions
	while(1){}
}

void	TIMER1_COMPA_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	TIMER1_COMPA_vect(void)
{
	static uint8_t color_table[7] =
	{
		_BV(RED), _BV(GREEN), _BV(BLUE),
		YELLOW, CYAN, PINK,
		WHITE
	};
	static int i = 0;

	display_color(color_table[i]);
	i++;

    if (i >= 7)
		i = 0;
}
