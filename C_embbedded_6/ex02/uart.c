#include "uart.h"

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

static const char base[17] = "0123456789ABCDEF";

void uart_init()
{
	// To compute UBRR (UART BAUD RATE REGISTER), the formula is as followed : (F_CPU / (BAUD * NOCB)). NOC = Number of Cycle per bits
	UBRR0 = (F_CPU + UART_BAUDRATE * 8UL) / (UART_BAUDRATE * 16UL) - 1; // compute UBBR and add (divisor / 2) to the divided value to round he number.
	// UCSRnC (UART Control and Status Register C), manages the transmitions options (synchonous/asynchronous, parity, stop bits, character size)
	// UCSZ (UART Character SiZe), controls the character size, here we activate the first and second bit which eq to 0
	SET_BIT(UCSR0C, UCSZ00); // activate first bit
	SET_BIT(UCSR0C, UCSZ01); // activate second bit
	CLEAR_BIT(UCSR0C, USBS0); // clear the bit of USBSn (UART Stop Bit Select) to put the stop bit at 1 (not requiered since it is supposed to be at 0 but just in case)
	// RXEN0 enables the receiver, TXEN0 enables de transmitter, RXCIE0 enables an Interruptor when we have something to reed in UART0we will use that with the UART_RX_vect function in ISR to properly recieve a message and not block entirely our program
	UCSR0B |= _BV(TXEN0);
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

uint8_t uart_rx(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}