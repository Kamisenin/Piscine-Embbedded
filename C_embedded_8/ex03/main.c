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

int	main(void)
{
	CLEAR_INTERRUPT;
	setup();
	SET_INTERRUPT;
	while(1)
		;
}

void	setup(void)
{
	/* SPI SETUP */
	DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
	SPCR = _BV(SPE) | _BV(MSTR)| _BV(SPR0);

	/* ADC SETUP */
	PRR &= ~_BV(PRADC);
	ADMUX = _BV(REFS0) | _BV(ADLAR);
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	ADCSRB = _BV(ADTS2) | _BV(ADTS0);


	/*Timer 1 Setup*/
	OCR1A = 5000;
	OCR1B = 5000; // start conversion every 20ms
	TIMSK1 = _BV(OCIE1B); 
	TCCR1B |= PRESCALE_64 | CTC_1;
}

void	ADC_vect(void) __attribute__((NO_INTERRUP, USED, VISIBLE));

void	ADC_vect(void)
{
	uint16_t p = (uint32_t)ADCH * 100UL / 255;
	uint32_t leds[3] = { 
		(p >=  33) ? MASK_BR(RED) : 0,
		(p >= 66) ? MASK_BR(RED) : 0,
		(p >= 100) ? MASK_BR(RED) : 0};
	display_color(leds, 3);
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