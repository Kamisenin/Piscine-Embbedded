#include "eeprom.h"


static inline _Bool	is_printable(unsigned char c);


void    eeprom_hexdump(const uint16_t start, const uint16_t stop)
{
	for (uint16_t eeprom_address = start; eeprom_address <= stop; eeprom_address += 16)
	{
		unsigned char    str[17] = "                ";
		uart_putnbr_base_width(eeprom_address, 16, 7, '0');
		uart_tx(' ');
		for (uint8_t i = 0; i < 16; i++)
		{
			str[i] = eeprom_read(eeprom_address + i);
			uart_putnbr_base_width(str[i], 16, 2, '0');
			uart_tx(' ');
			if (!is_printable(str[i]))
				str[i] = '.';
		}
		uart_tx('|');
		uart_print_u_str(str);
		uart_printstr("|\r\n");
	}
}


uint8_t eeprom_read(uint16_t address)
{
	eeprom_busy();
	/* Set up address register */
	EEAR = address;
	/* Start eeprom read by writing EERE */
	SET_BIT(EECR, EERE);
	/* Return data from Data Register */
	return EEDR;
}

void    eeprom_busy()
{
	while(EECR & _BV(EEPE))
		;
}


_Bool	is_printable(unsigned char c)
{
	return (c >= 32 && c <= 126);
}
