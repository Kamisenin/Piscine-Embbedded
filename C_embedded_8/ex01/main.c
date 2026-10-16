#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "bit_operation.h"
#include "interrupt.h"
#include "timer.h"

#define RED 255UL
#define GREEN (255UL << 8) 
#define BLUE (255UL << 16)
#define BRIGHTNESS (10UL << 24)  // Brigthness of 20%
#define YELLOW (RED | GREEN)
#define CYAN (GREEN | BLUE)
#define PINK (RED | BLUE)
#define WHITE (RED | GREEN | BLUE)

#define MASK_BR(color) (BRIGHTNESS | color)

static void			setup(void);
static void			display_color(const uint32_t *colors, const uint8_t n);
static void			color(uint8_t brightness, const uint8_t blue, const uint8_t green,const uint8_t red);
static inline void	spi_send(const uint8_t data);



int	main(void)
{
	CLEAR_INTERRUPT;
	setup();
	SET_INTERRUPT;
	while(1){}
}

void	setup(void)
{
	DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
	SPCR = _BV(SPE) | _BV(MSTR)| _BV(SPR0);

	SET_BIT(TCCR1B, WGM12); // CTC mode
	SET_BIT(TIMSK1, OCIE1A); // enable the OCIE1A Output Compare A Match Interrupt
	OCR1A = 62499; // OCR1A = ((F_CPU * Time_in_seconds) / prescaler) - 1
	TCCR1B |= PRESCALE_256; // select prescaler to start the timer (prescaler of 256)
}

void	TIMER1_COMPA_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	TIMER1_COMPA_vect(void)
{
	static uint8_t 			i = 0;
	static const uint32_t	color_table[7] =
	{
		MASK_BR(RED), MASK_BR(GREEN), MASK_BR(BLUE),
		MASK_BR(YELLOW), MASK_BR(CYAN), MASK_BR(PINK),
		MASK_BR(WHITE)
	};
	static uint32_t led_list[3] = {0};

	led_list[0] = color_table[i];
	display_color(led_list, 3);
	i++;
	if (i == 7)
		i = 0;
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