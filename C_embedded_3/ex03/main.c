#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "rgb.h"
#include "uart.h"
#include "interrupt.h"

#define BACKSPACE 127

int main()
{
	CLEAR_INTERRUPT;
	uart_init();	
	init_rgb();
	SET_INTERRUPT;
	uart_printstr("Enter color code : ");
	while (1);
}

void clear_buff(char *buff)
{
	while (*buff)
	{
		*buff = '\0';
		buff++;
	}
}

void handle_keys(char key)
{
	static char buff[8] = {0};
	static uint8_t i = 0;

	if (i == 7 && key != '\r' && key != BACKSPACE)
		return ;
	if (i == 0 && key == BACKSPACE)
		return ;

	switch(key) {
		case '\r':
			uart_printstr("\r\n");
			set_rgb(get_rgb_from_hex(buff));
			i = 0;
			clear_buff(buff);
			uart_printstr("Enter color code : ");
			break;
		case BACKSPACE:
			uart_printstr("\b \b");
			buff[--i] = '\0';
			break;
		default :
			uart_tx(key);
			buff[i++] = key;
	}
}

void	USART_RX_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	USART_RX_vect(void)
{
	handle_keys(UDR0);
}