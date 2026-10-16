#include "eeprom.h"


static inline _Bool	is_printable(unsigned char c);


void    eeprom_hexdump(const uint16_t start, const uint16_t stop)
{
	for (uint16_t eeprom_address = start; eeprom_address < stop; eeprom_address += 16)
	{
		unsigned char    str[17] = "                ";
		uart_putnbr_base_width(eeprom_address, 2, 10, '0');
		uart_tx(' ');
		for (uint8_t i = 0; i < 16; i++)
		{
			str[i] = eeprom_print_hex(eeprom_address);
		}
		uart_tx('|');
		uart_print_u_str(str);
		uart_printstr("|\r\n");
	}
}

char	eeprom_print_hex(uint16_t address)
{
	unsigned char c = eeprom_read(address);
	uart_putnbr_base_width(c, 16, 2, '0');
	if (!is_printable(c))
		return ('.');
	return (c);

}

uint8_t eeprom_read(const uint16_t address)
{
	eeprom_busy();
	/* Set up address register */
	EEAR = address;
	/* Start eeprom read by writing EERE */
	SET_BIT(EECR, EERE);
	/* Return data from Data Register */
	return EEDR;
}

void	eeprom_read_block(const void *dst, const uint16_t address, const uint8_t size)
{
	unsigned char *dest = (unsigned char *) dst;
	for (uint8_t i = 0; i < size; i++)
	{
		dest[i] = eeprom_read(address + i);
	}
}

_Bool	eeprom_write_block(const void *src, const uint16_t address, const uint8_t size)
{
	unsigned char *block = (unsigned char *) src;
	for (uint8_t i = 0; i < size; i++)
	{
		if (eeprom_safe_write(address + i, block[i]))
			return (1);
	}
	return (0);
}

void eeprom_write(const uint16_t address, const uint8_t data)
{
	eeprom_busy();
	/* Set up address and Data Registers */
	EEAR = address;
	EEDR = data;
	/* Write logical one to EEMPE */
	SET_BIT(EECR, EEMPE);
	/* Start eeprom write by setting EEPE */
	SET_BIT(EECR, EEPE);
}

_Bool	eeprom_safe_write(const uint16_t address, const uint8_t data)
{
	if (eeprom_read(address) == data)
		return (0);
	EEAR = address;
	EEDR = data;
	SET_BIT(EECR, EEMPE);
	SET_BIT(EECR, EEPE);
	if (eeprom_read(address) != data)
		return (1);
	return (0);
}

void    eeprom_busy()
{
	/* Wait for completion of previous write */
	while(EECR & _BV(EEPE))
		;
}


_Bool	is_printable(unsigned char c)
{
	return (c >= 32 && c <= 126);
}