#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "utils.h"

#define BIT_MASK(bit) ((1 << bit))
#define SET_BIT(REGISTER, bit) (REGISTER |= BIT_MASK(bit))
#define CLEAR_BIT(REGISTER, bit) (REGISTER &= ~BIT_MASK(bit))
#define LOOP while(1)

static void uart_init()
{
    // To compute UBRR (UART BAUD RATE REGISTER), the formula is as followed : (F_CPU / (BAUD * NOCB)). NOC = Number of Cycle per bits
    UBRR0L = (F_CPU + UART_BAUDRATE * 8UL) / (UART_BAUDRATE * 16UL) - 1; // compute UBBR and add (divisor / 2) to the divided value to round he number. We only use UBBRnL because the subject requires us to
    // UCSRnC (UART Control and Status Register C), manages the transmitions options (synchonous/asynchronous, parity, stop bits, character size)
    // UCSZ (UART Character SiZe), controls the character size, here we activate the first and second bit which eq to 0
    SET_BIT(UCSR0C, UCSZ00); // activate first bit
    SET_BIT(UCSR0C, UCSZ01); // activate second bit
    CLEAR_BIT(UCSR0C, USBS0); // clear the bit of USBSn (UART Stop Bit Select) to put the stop bit at 1 (not requiered since it is supposed to be at 0 but just in case)
     // RXEN0 enables the receiver, TXEN0 enables de transmitter, RXCIE0 enables an Interruptor when we have something to reed in UART0we will use that with the UART_RX_vect function in ISR to properly recieve a message and not block entirely our program
    UCSR0B |= BIT_MASK(TXEN0) | BIT_MASK(RXEN0) | BIT_MASK(RXCIE0);
}

static inline void uart_tx(char c)
{
    /*
     * To transmit, UART has a register called UDRn that we can give our data to.
     * But we need to wait for the previous data to be transfered, which is why
     * it has a register UCSRnA (USART Control and Status Register A),
     * which manages the transmission states. Here we are interested in
     * UDREn (UART Data Register Empty), which check if the UDR (UART Data register)
     * is empty 0 meaning the data receiver is not empty.
     */
    while ( !( UCSR0A & (1<<UDRE0)) ); // wait until info has been sent

    UDR0 = c; // send the data to the register
}

/*
 * In the Documentation the UART cable correspond to
 * the USART (UNIVERSAL SYNCHRONOUS and ASYNCHRONOUS serial RECIEVER and TRANSMITTER)
 * its initialization process normally consists of setting the baud rate,
 * setting frame format and enabling the Transmitter or the Receiver depending on the usage
 */
int main()
{
    cli();
    uart_init();
    sei();
    LOOP;
}


/*
 * Here we use USART_RX_vect which is the interruption event we want to catch for the ATMega328p (the vector depend on the hardware used)
 */
ISR(USART_RX_vect)
{
    char c = UDR0;

    if (ft_isalpha(c))
    {
        if (ft_is_caps(c))
            c += 32;
        else
            c -= 32;
    }
    uart_tx(c);
}