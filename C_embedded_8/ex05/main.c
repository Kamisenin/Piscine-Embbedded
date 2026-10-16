#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "bit_operation.h"
#include "interrupt.h"
#include "rgb.h"
#include <util/delay.h>

#define BACKSPACE 127

static void		setup(void);
static uint8_t	adc_convert();
static void 	temp_color();
static uint32_t set_channel(uint32_t color, uint8_t channel, uint8_t value);

static volatile uint32_t 	rv1_value = 0;
static volatile uint32_t	led_colors[3] = {BRIGHTNESS, BRIGHTNESS, BRIGHTNESS};
static volatile uint8_t		led_index = 0;
static volatile uint8_t		current_color = 0;

static uint32_t tmp[3];


int	main(void)
{
	CLEAR_INTERRUPT;
	setup();
	SET_INTERRUPT;
	while(1)
	{
		rv1_value = adc_convert();
		temp_color();
		display_color(tmp, 3);
		_delay_ms(20);
	};
}

void	setup(void)
{
	init_rgp_spi();

	/* SW1 & SW2 handlers*/
	PCMSK2 |= _BV(PCINT20) | _BV(PCINT18);
	PCICR |= _BV(PCIE2);

	/* ADC SETUP */
	PRR &= ~_BV(PRADC);
	ADMUX = _BV(REFS0) | _BV(ADLAR);
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

uint8_t	adc_convert()
{
	ADCSRA |= _BV(ADSC); // start conversion (ADSC = ADC Start Conversion)
	while (ADCSRA & _BV(ADSC)); //wait for conversion to be done (ADSC = 0)
	return (ADCH);
}

void	PCINT2_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	PCINT2_vect(void)
{
	static uint8_t	sw1_state = 1;
	static uint8_t	sw2_state = 1;

	_delay_ms(20);

	uint8_t sw1 = (PIND >> PIND2) & 1;
	uint8_t sw2 = (PIND >> PIND4) & 1;

	if (sw1 != sw1_state) {
		if (sw1 == 0)
		{
			led_colors[led_index] = set_channel(led_colors[led_index], current_color, rv1_value);
			display_color(led_colors, 3);
			current_color++;
			if (current_color == 3)
				current_color = 0;
		}
		sw1_state = sw1;
	}
	if (sw2 != sw2_state) {
		if (sw2 == 0)
		{
			led_index++;
			if (led_index > 2)
				led_index = 0;
			current_color = 0;
		}
		sw2_state = sw2;
	}
	
	/* PCIFR = Pin Change Interrupt Flag Register, it handles the Interrupt flags, putting a logical 1 into one flag cancel its interrupt queue */
	PCIFR |= _BV(PCIF2); //cancel vector queue to handle rebounce
}

void temp_color()
{
	for (uint8_t i = 0; i < 3; i++)
	{
		if (i == led_index)
			tmp[i] = set_channel(led_colors[i], current_color, rv1_value);
		else
			tmp[i] = led_colors[i];
	}
}

uint32_t set_channel(uint32_t color, uint8_t channel, uint8_t value)
{
    uint8_t shift = channel * 8;
    return (color & ~((uint32_t)0xFF << shift)) | ((uint32_t)value << shift);
}
