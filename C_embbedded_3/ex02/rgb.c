#include "bit_operation.h"

uint8_t luminosity = 255;

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = (uint16_t) r * luminosity / 255;
	OCR0A = (uint16_t) g * luminosity / 255;
	OCR2B = (uint16_t) b * luminosity / 255;
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
		set_rgb(255 - pos * 3, 0, pos * 3);
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} 
	else
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

void init_rgb()
{
	DDRD |= _BV(DDD3) | _BV(DDD5) | _BV(DDD6);
	PORTD |= _BV(PORTD3) | _BV(PORTD5) | _BV(PORTD6);

	TCCR0A |= _BV(WGM00) | _BV(COM0A1) | _BV(COM0B1); // PWM Phase correct and LEDs
	//SET_BIT(TIMSK0, TOIE0); // enable the OCIE1A Output Compare A Match Interrupt
	 // select prescaler to start the timer (prescaler of 8)

	TCCR2A |= _BV(WGM00) |_BV(COM2B1); // PWM Phase correct and LEDs
	//SET_BIT(TIMSK0, TOIE0); // enable the OCIE1A Output Compare A Match Interrupt
	SET_BIT(TCCR2B, CS21); // select prescaler to start the timer (prescaler of 8)
	SET_BIT(TCCR0B, CS01);
}


void set_luminosity(uint8_t l)
{
    luminosity = l;
}