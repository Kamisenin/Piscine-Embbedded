#include "bit_operation.h"
#include "rgb.h"

uint8_t luminosity = 255;

void set_rgb(t_rgb rgb)
{
	OCR0B = (uint16_t) rgb.r * luminosity / 255;
	OCR0A = (uint16_t) rgb.g * luminosity / 255;
	OCR2B = (uint16_t) rgb.b * luminosity / 255;
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
		set_rgb((t_rgb){255 - pos * 3, 0, pos * 3});
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb((t_rgb){0, pos * 3, 255 - pos * 3});
	} 
	else
	{
		pos = pos - 170;
		set_rgb((t_rgb){pos * 3, 255 - pos * 3, 0});
	}
}

void init_rgb()
{
	DDRD |= _BV(DDD3) | _BV(DDD5) | _BV(DDD6);
	PORTD |= _BV(PORTD3) | _BV(PORTD5) | _BV(PORTD6);

	TCCR0A |= _BV(WGM00) | _BV(COM0A1) | _BV(COM0B1); // PWM Phase correct and LEDs

	TCCR2A |= _BV(WGM00) |_BV(COM2B1); // PWM Phase correct and LEDs
	SET_BIT(TCCR2B, CS21); // select prescaler to start the timer (prescaler of 8)
	SET_BIT(TCCR0B, CS01);
}

void set_luminosity(uint8_t l)
{
    luminosity = l;
}
