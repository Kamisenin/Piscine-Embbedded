#include <avr//io.h>
#include <util/delay.h>

static inline void	spi_send(uint8_t data);
static void			color(uint8_t brightness, uint8_t blue, uint8_t green, uint8_t red);


int main(void)
{
	DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
	SPCR = _BV(SPE) | _BV(MSTR)| _BV(SPR0); // enable SPI in Master mode, prescaler 16

	while (1)
	{
		// send 32 bits 0 to signify start to LED Datasheet (https://www.alldatasheet.com/datasheet-pdf/view/1150589/ETC2/APA102.html) p.4
		for (uint8_t i = 0; i < 4; i++)
		{
			spi_send(0x00);
		}
		// since we have 3 led we send 3 times 32 bits, since we only want D6 which is first we dont send any other 
		color(20, 0, 0, 150);
		color(0, 0, 0, 0);
		color(0, 0, 0, 0);
		// stop bits
		for (uint8_t i = 0; i < 4; i++)
		{
			spi_send(0xFF);
		}
		_delay_ms(100);
	}
}

void	color(uint8_t brightness, uint8_t blue, uint8_t green, uint8_t red)
{
	if (brightness > 100)
		brightness = 100;
	spi_send(0xE0 | ((brightness * 31 + 50) / 100));
	spi_send(blue);
	spi_send(green);
	spi_send(red);
}

void spi_send(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}