#include "rgb.h"
#include "bit_operation.h"

static void spi_send(const uint8_t data);

void    init_rgp_spi()
{
    DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
	SPCR = _BV(SPE) | _BV(MSTR)| _BV(SPR0);
}

void	display_color(const volatile uint32_t *colors, const uint8_t n)
{
	//start bits
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

uint32_t	wheel(uint32_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
		return( MASK_BR( (255UL - pos * 3UL) | ((pos * 3) << 16UL) ) );
	else if (pos < 170)
	{
		pos = pos - 85;
		return( MASK_BR( ((pos * 3) << 8UL) | ((255UL - pos * 3UL) << 16UL) ) );
	} 
	else
	{
		pos = pos - 170;
		return( MASK_BR( (pos * 3UL) | ((255UL - pos * 3UL) << 8UL) ) );
	}
}