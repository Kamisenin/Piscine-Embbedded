#include "eeprom.h"
#include "interrupt.h"
#include "utils.h"
#include "network_node/network_node.h"

static void		handle_key(unsigned char c);
static void		process_prompt(unsigned char *str);
static void		clear_buf(unsigned char *buff);

t_node	node = {0};

int main(void)
{
	CLEAR_INTERRUPT;
	uart_init();
	node_init(&node);
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
	static unsigned char	buf[43] = {0};
	static uint8_t			i = 0;
	
	switch (c)
	{
	case BACKSPACE:
		if (i <= 0)
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
		if (!is_printable(c) || i > 41)
			return ;
		uart_tx(c);
		buf[i++] = c;
		break;
	}
}

void process_prompt(unsigned char *str)
{

	if (!ft_strncmp(str, "STATUS", 6))
		status(&node);
	else if (!ft_strncmp(str, "SET_ID ", 7))
		set_id(str + 7, &node);
	else if (!ft_strncmp(str, "SET_PRIO ", 9))
		set_prio(str + 9, &node);
	else if (!ft_strncmp(str, "SET_TAG ", 8))
		set_tag(str + 8, &node);
	else if (!ft_strncmp(str, "FACTORY_RESET", 14))
		factory_reset(&node);
	else
		uart_printstr("Invalid command\r\n");
}

void	clear_buf(unsigned char *buff)
{
	while (*buff)
	{
		*buff = 0;
		buff++;
	}
}