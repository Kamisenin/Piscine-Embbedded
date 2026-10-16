#include "eeprom.h"
#include "interrupt.h"
#include "utils.h"

static void		handle_key(unsigned char c);
static void		process_prompt(unsigned char *str);
static _Bool	is_already_written(uint16_t address, uint8_t value);
static void		print_eeprom_change(uint8_t address);
static char		color_on_address(uint16_t address, uint16_t current);
static void		clear_buf(unsigned char *buff);


int main(void)
{
	CLEAR_INTERRUPT;
	uart_init();
	uart_printstr("> ");
	SET_INTERRUPT;
	while (1)
        ;
}

void	USART_RX_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	USART_RX_vect(void)
{
	handle_key(UDR0);
}

void handle_key(unsigned char c)
{
	static unsigned char	buf[7] = {0};
	static uint8_t			i = 0;
	
	switch (c)
	{
	case BACKSPACE:
		if (i == 0)
			return;
		buf[i--] = '\0';
		uart_printstr("\b \b");
		break;
	
	case '\r':
		uart_printstr("\r\n");
		process_prompt(buf);
		uart_printstr("> ");
		clear_buf(buf);
		i = 0;
		break;
	
	default:
		if (!is_printable(c) || i >= 6)
			return ;
		uart_tx(c);
		buf[i++] = c;
		break;
	}
}

void process_prompt(unsigned char *str)
{
	uint16_t	address;
	uint8_t		value;
	uint8_t 	i = 0;
	
	if (!is_prompt_valid(str))
	{
		print_red("Invalid Arguments\r\n");
		return ;
	}
	address = ahtoi(str, &i);
	if (address > 0x3FF)
	{
		uart_printstr("Invalid address\r\n");
		return ;
	}
	i++;
	value = ahtoi(str, &i);
	if (is_already_written(address, value))
		return ;
	eeprom_write(address, value);
	print_eeprom_change(address);
}

_Bool	is_already_written(uint16_t address, uint8_t value)
{
	return (eeprom_read(address) == value);
}

void	print_eeprom_change(uint8_t address)
{
	for (uint16_t eeprom_address = 0x00; eeprom_address <= 0x3FF; eeprom_address += 16)
	{
		unsigned char    str[17] = {0};
		uart_putnbr_base_width(eeprom_address, 16, 7, '0');
		uart_tx(' ');
		for (uint8_t i = 0; i < 16; i += 2)
		{
			str[i] = color_on_address(address, eeprom_address + i);
			str[i + 1] = color_on_address(address, eeprom_address + i + 1);
			uart_tx(' ');
		}
		uart_tx('|');
		uart_print_u_str(str);
		uart_printstr("|\r\n");
	}
}

char color_on_address(uint16_t address, uint16_t current)
{
	unsigned char c;
	if (current == address)
	{	
		uart_printstr(RED_COLOR);	
		c = eeprom_print_hex(current);
		uart_printstr(CLEAR_COLOR);
	}
	else
		c = eeprom_print_hex(current);
	return (c);
}

void	clear_buf(unsigned char *buff)
{
	while (*buff)
	{
		*buff = 0;
		buff++;
	}
}
