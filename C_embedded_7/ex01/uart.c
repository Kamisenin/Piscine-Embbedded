#include "uart.h"

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

const char base[17] = "0123456789abcdef";

void uart_init()
{
	UBRR0 = (F_CPU + UART_BAUDRATE * 8UL) / (UART_BAUDRATE * 16UL) - 1;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UCSR0B |= _BV(TXEN0)  | _BV(RXEN0) | _BV(RXCIE0);
}

void uart_tx(char c)
{
	while ( !( UCSR0A & (1<<UDRE0)) ); // wait until info has been sent

	UDR0 = c; // send the data to the register
}

void uart_printstr(const char* str)
{
	while (*str)
	{
		uart_tx(*str);
		str++;
	}
}

void uart_print_u_str(const unsigned char* str)
{
	while (*str)
	{
		uart_tx(*str);
		str++;
	}
}

void uart_putnbr_base(uint16_t n, uint8_t base_length)
{
	if (n >= base_length)
	{
		uart_putnbr_base(n / base_length, base_length);
		uart_putnbr_base(n % base_length, base_length);
	}
	else
		uart_tx(base[n]);
}

void uart_putnbr_base_width(uint16_t n, uint8_t base_len, int width, unsigned char pad)
{
    char buf[32];
    int  i = 0;

    do {
        buf[i++] = base[n % base_len];
        n /= base_len;
    } while (n);

    while (width-- > i)
        uart_tx(pad);
    while (i--)
        uart_tx(buf[i]);
}

uint8_t uart_rx(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}