#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "bit_operation.h"
#include "interrupt.h"
#include "uart.h"
#include "rgb.h"
#include "utils.h"

#define BACKSPACE 127


static void		setup(void);
static void		handle_keys(char key);
static void		clear_buff(unsigned char *buff);
static void		process_prompt(unsigned char *cli);
static _Bool	is_hex(unsigned char *cli);

int	main(void)
{
	CLEAR_INTERRUPT;
	setup();
	SET_INTERRUPT;
	uart_printstr("Enter color code : ");
	while(1)
		;
}

void	setup(void)
{
	init_rgp_spi();
	uart_init();
}

void	USART_RX_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	USART_RX_vect(void)
{
	handle_keys(UDR0);
}

void handle_keys(char key)
{
	static unsigned char	buff[13] = {0};
	static uint8_t			i = 0;

	switch(key) {
		case '\r':
			uart_printstr("\r\n");
			if (i == 0)
			{
				uart_printstr("Enter color code : ");
				break;
			}
			i = 0;
			process_prompt(buff);
			clear_buff(buff);
			uart_printstr("Enter color code : ");
			break;
		case BACKSPACE:
			if (i <= 0)
				return;
			uart_printstr("\b \b");
			buff[--i] = '\0';
			break;
		default :
			if (!is_printable(key) || i > 11)
				return;
			uart_tx(key);
			buff[i++] = key;
	}
}

void clear_buff(unsigned char *buff)
{
	while (*buff)
	{
		*buff = '\0';
		buff++;
	}
}

void	process_prompt(unsigned char *cli)
{
	static uint32_t leds[3] = {0};

	if (*cli != '#')
	{
		uart_printstr("Invalid command, usage : #RRGGBBDx\r\n");
		return ;
	}
	if (!ft_strncmp(cli + 1, "FULLRAINBOW", 11))
	{
		set_rainbow();
		return ;
	}
	if (!is_hex(cli + 1))
	{
		uart_printstr("Invalid command, usage : #RRGGBBDx\r\n");
		return ;
	}
	if (cli[8] == '\0')
	{
		uart_printstr("Invalid command, usage : #RRGGBBDx\r\n");
		return ;
	}

	uint32_t rgb = conver_hex_to_rgb(cli + 1);

	if (!ft_strncmp(cli + 8, "6", 2))
		leds[0] = rgb;
	else if (!ft_strncmp(cli + 8, "7", 2))
		leds[1] = rgb;
	else if (!ft_strncmp(cli + 8, "8", 2))
		leds[2] = rgb;
	else
	{
		uart_printstr("Invalid command, usage : #RRGGBBDx\r\n");
		return ;
	}
	if (get_rainbow())
		stop_rainbow();
	display_color(leds, 3);
}


_Bool	is_hex(unsigned char *cli)
{
	uint8_t i;

	for (i = 0; i < 6 && cli[i] ; i++)
	{
		if (!is_hex_digit(cli[i]))
			return (0);
	}
	if (i < 6 || cli[i] != 'D')
		return (0);
	return (1);
}