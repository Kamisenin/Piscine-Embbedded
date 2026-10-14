#include "bit_operation.h"
#include "utils.h"
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

t_rgb get_rgb_from_hex(char *hex)
{
	t_rgb rgb = {0};
	if (*hex != '#')
		return (rgb);
	uint8_t i = 1;
	while (hex[i])
	{
		if (!ft_is_in_str(hex[i], HEX))
			return (rgb); 
		i++;
	}
	if (i < 7)
		return (rgb);
	rgb.r = ft_atoi_n_base(hex + 1, hex + 3, HEX);
	rgb.g = ft_atoi_n_base(hex + 3, hex + 5, HEX);
	rgb.b = ft_atoi_n_base(hex + 5, hex + 7, HEX);
	return (rgb);
}