#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "bit_operation.h"
#include "interrupt.h"
#include "timer.h"
#include "util/delay.h"

#define RED 255UL
#define GREEN (255UL << 8) 
#define BLUE (255UL << 16)
#define BRIGHTNESS (10UL << 24)  // Brigthness of 20%

#define MASK_BR(color) (BRIGHTNESS | color)

static void			setup(void);
static void			display_color(const uint32_t *colors, const uint8_t n);
static void			color(uint8_t brightness, const uint8_t blue, const uint8_t green,const uint8_t red);
static inline void	spi_send(const uint8_t data);

volatile int16_t	led_timers[3] = {
	0, 0, 0
};

volatile _Bool refresh = 1;

int	main(void)
{
	CLEAR_INTERRUPT;
	setup();
	SET_INTERRUPT;
	while(1)
	{		
		for (uint8_t i = 0; i < 3; i++)
		{
			CLEAR_INTERRUPT;
			led_timers[i] = 250;
			refresh = 1;
			SET_INTERRUPT;
			_delay_ms(333); // 333.333 millisecond
		}
	}
}

void	setup(void)
{
	DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
	SPCR = _BV(SPE) | _BV(MSTR)| _BV(SPR0);

	SET_BIT(TCCR1B, WGM12); // CTC mode
	SET_BIT(TIMSK1, OCIE1A); // enable the OCIE1A Output Compare A Match Interrupt
	OCR1A = 6250; // every 25 ms
	TCCR1B |= PRESCALE_64;
}

void	TIMER1_COMPA_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	TIMER1_COMPA_vect(void)
{
	_Bool state_changed = refresh;
	refresh = 0;

	for (uint8_t i = 0; i < 3; i++)
	{
		if (led_timers[i] > 0 && (led_timers[i] -= 25) == 0)
			state_changed = 1;
	}

	if (state_changed)
	{
		uint32_t led_light[3];
		for (uint8_t i = 0; i < 3; i++)
			led_light[i] = led_timers[i] > 0 ? MASK_BR(0x3F) : 0;
		display_color(led_light, 3);
	}
}


void	color(uint8_t brightness, const uint8_t blue, const uint8_t green, const uint8_t red)
{
	if (brightness > 100)
		brightness = 100;
	spi_send(0xE0 | ((brightness * 31 + 50) / 100));
	spi_send(blue);
	spi_send(green);
	spi_send(red);
}

void spi_send(const uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}

void	display_color(const uint32_t *colors, const uint8_t n)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		spi_send(0x00);
	}
	for (uint8_t i = 0; i < n; i++)
	{
		color(colors[i] >> 24, (colors[i] & BLUE) >> 16, (colors[i] & GREEN) >> 8, colors[i] & RED);
	}
	// stop bits
	for (uint8_t i = 0; i < 4; i++)
	{
		spi_send(0xFF);
	}
}