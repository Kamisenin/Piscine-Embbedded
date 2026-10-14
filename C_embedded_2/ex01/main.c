#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

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
    SET_BIT(UCSR0B, TXEN0); // TXENO enables the transmitter
}

static inline void uart_tx(char c)
{
    /*
     * To transmit, UART has a register called UDRn that we can give our data to.
     * But we need to wait for the previous data to be transfered, which is why
     * it has a register UCSRnA (USART Control and Status Register A),
     * which manages the transmission states. Here we are interested in
     * UDREn (UART Data Register Empty), which check if the UDR (UART Data register)
     * is empty 0 meaning the data reciever is not empty.
     */
    while ( !( UCSR0A & (1<<UDRE0)) ); // wait until info has been sent

    UDR0 = c; // send the data to the register
}

static void uart_printstr(const char* str)
{
    while (*str)
    {
        uart_tx(*str);
        str++;
    }
}

void setup()
{
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6); // put Data Direction of D3, D5 and D, the r, g and b led to output mode

	SET_BIT(TCCR1B, WGM12); // CTC mode
	SET_BIT(TIMSK1, OCIE1A); // enable the OCIE1A Output Compare A Match Interrupt
	OCR1A = 31250; // OCR1A = ((F_CPU * Time_in_seconds) / prescaler) - 1
    CLEAR_BIT(PRR, PRTIM1); // PRTIMn of PRR (Power Reduction Register) needs to be explicitly set to 0 because otherwise if controller enters sleep mode needs lots of power it will stop the clock
    uart_init(); // initialize UART
	TCCR1B |= BIT_MASK(CS10) | BIT_MASK(CS12); // select prescaler to start the timer (prescaler of 1024)
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
    setup();
    sei();
    LOOP;
}



/*
 * Interrupts are functions defined in a specific space of the hardware, they are space reserved that can be redefined but never moved
 * Depending on the Interrupt Vector used it will be called for different events. In our specific case, we want the interruption to be
 * called every time timer1 reaches OCR1A, so we use TIMER1_COMPA that we found at page 66 of the chip's documentation
 * Another important thing, while an interrupt function is ongoing, it cannot be interrupted by another interrupt with Higher priority,
 * except if we explicitely told him that he can be interrupt with sei()
 */
ISR(c) {
    uart_printstr("Hello World!\r\n");
}
