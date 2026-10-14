#ifndef F_CPU
# define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200
#endif

#include "utils.h"

const char username[5] = "Kami";
const char password[14] = "42chipsIsC00L";

t_state state = {0};

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

void reset()
{
    state.username_last_valid_addr = 0;
    state.password_last_valid_addr = 0;
    state.validity = BIT_MASK(USV) | BIT_MASK(PSV);
    state.current_username_address = (char *) username;
    state.current_password_address = (char *) password;
    CLEAR_BIT(PORTD, PORTD5);
    CLEAR_BIT(PORTD, PORTD6);
    uart_printstr("Welcome, please enter your credentials\r\n");
    uart_printstr("username : ");
}

void setup()
{
    // enable Data Direction output mode of the leds for the animations (Dramatic effects !!)
    DDRD |= BIT_MASK(DDD5) | BIT_MASK(DDD6);
    DDRB |= BIT_MASK(DDB0) | BIT_MASK(DDB1) | BIT_MASK(DDB2 ) | BIT_MASK(DDB4);
    uart_init();
    reset();
}

void animation()
{
    PORTB |= (1 << PORTB0);
    _delay_ms(150);
    PORTB |= (1 << PORTB1);
    _delay_ms(150);
    PORTB |= (1 << PORTB2);
    _delay_ms(150);
    PORTB |= (1 << PORTB4);
    _delay_ms(150);

    for (uint8_t i = 0; i < 3; i++)
    {
        PORTB &= ~LED_MASK_PORTB; // On
        _delay_ms(200);

        PORTB |= LED_MASK_PORTB;  // OFF
        _delay_ms(200);
    }

    PORTB &= ~LED_MASK_PORTB;

}

void success()
{
    animation();
    SET_BIT(state.validity, CONNECTED);
    SET_BIT(PORTD, PORTD6);
    uart_printstr("Successfully connected\r\n");
}

void failure()
{
    animation();
    SET_BIT(PORTD, PORTD5);
    uart_printstr("Invalid Credentials\r\n");
    _delay_ms(1000);
    uart_printstr("Try again...\r\n");
    reset();
}

int main()
{
    cli();
    setup();
    sei();
    LOOP;
}

void handle_keys(char c)
{
    if (c == '\0')
        reset();
    else if (state.validity & BIT_MASK(CONNECTED) && c != '\r') // if user is connected
    {
        if (c == BACKSPACE)
            uart_printstr("\b \b");
        else
            uart_tx(c);
    }
    else if (c == '\r')
        uart_printstr("\r\n");
    else if (state.validity & BIT_MASK(CS) && c != BACKSPACE)
        uart_tx('*');
    else if (c != BACKSPACE)
        uart_tx(c);
}


/*
 * Catch USART_RX_vect to listen to values coming in 
 */
ISR(USART_RX_vect)
{
    char c = UDR0;

    handle_keys(c);
    if (state.validity & BIT_MASK(CONNECTED))
        return ;

    switch(c)
    {
        case '\r':
            if (state.validity & (BIT_MASK(CS) | BIT_MASK(USV) | BIT_MASK(PSV)) && *state.current_password_address == '\0' && *state.current_username_address == '\0')
                success();
            else if ( !(state.validity & (BIT_MASK(CS))) )
            {
                state.validity |= BIT_MASK(CS);
                uart_printstr("password : ");
            }
            else
                failure();
            break;
        case BACKSPACE:
            manage_backspace();
            break;
        default:
            if (state.validity & BIT_MASK(CS))
                manage_password_validity(c);
            else
                manage_username_validity(c);
    }
}