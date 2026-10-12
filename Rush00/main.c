#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "rgb.h"
#include "game.h"
#include <util/delay.h>

#define BACKSPACE 127
#define INIT_LED 

volatile t_state		status = {0};
volatile uint8_t		sw1_state = 1;
extern volatile uint8_t resolve_pending;

static void init_button()
{
	DDRD &= ~_BV(DDD2); // put SW1 Data Direction to input (security)
	PORTD |= _BV(PORTD2); // put SW1 pull up resistor to high
	PCMSK2 |= _BV(PCINT18); // activate interrupt of SW1
	PCICR |= _BV(PCIE2); // allow external interrupt from PCINT2 vector
}


void init_led()
{
	DDRB |= _BV(DDB0) | _BV(DDB1) | _BV(DDB2) | _BV(DDB4); // output data direction of leds
	PORTB &= ~(_BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB4)); // turn off led for security reason
}

int main()
{
	CLEAR_INTERRUPT;
	init_rgb();
	init_led();
	init_button();
	twi_init();	
	set_luminosity(100);
	SET_INTERRUPT;
	while (1);
}

void handle_slave()
{
	switch (TW_STATUS)
	{
		/* SEND DATA */
		case TW_ST_SLA_ACK: // Master waiting for data
			if (!status.waiting)
				status.waiting = 1;
			if (!(TCCR1B & (_BV(CS10) | _BV(CS11)))) // check that the prescaler has not been set before
        		start_slave_watchdog(); // start check clock on link in case the Master resets before game start
			TWDR = status.sl_button_pressed; // if its 0 Master will ask again for new infos, if its 1 we set to 0 as we have transferred info 
			status.sl_button_pressed = 0;
			TWCR = TW_LAST; // since we wont send more and the Master only wants 1 byte we dont ACK the reception
			break;

		case TW_ST_DATA_NACK: // When recieved NACK as slave transmitter, stop sending bytes
		case TW_ST_LAST_DATA: // IF Recieved ACK but stopped sending data (Error case not reachable in our case)
			TWCR = TW_SEND;
			break;

		/* RECIEVE DATA */
		// ! others DATA RECIEVE STATUS are already handled by the default
		case TW_SR_DATA_ACK: // BYTE RECIEVED
			handle_slave_led_info(TWDR);
			TWCR = TW_SEND; // ACK reception
			break;

		/* ERROR AND DEFAULT CASE*/
		case TW_BUS_ERROR: //if for some reason there is a problem with the BUS
			TWCR = TW_SEND | _BV(TWSTO); // stop bus and reset
			stop_game();
			break;

		default:
			TWCR = TW_SEND;
			break;
	}
}

void	PCINT2_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	PCINT2_vect(void)
{
	_delay_ms(50);

	uint8_t sw1 = (PIND >> PIND2) & 1;

	if (sw1 != sw1_state && sw1 == 0)
		return ;
	sw1_state = sw1;
	if (status.role == MASTER && status.started)
	{
		status.mt_button_pressed = 1;
		resolve_pending = 1;
	}
	else if (!status.waiting)
	{
		status.role = MASTER;
		status.waiting = 1;
		start_polling(POLL_FOREVER);
	}
	else if (status.role == SLAVE)
		status.sl_button_pressed = 1; // Slave Button was clicked, once Master request data it will send that the button has been clicked
	PCIFR |= _BV(PCIF2);
}

void	TWI_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	TWI_vect(void)
{
	if (status.waiting && status.role == MASTER) // used when waiting for the next timer change to listen to slave button triggering
		handle_poll();
	else if (status.role == MASTER && status.started) // case to manage data to send
		handle_master_tx(status.countdown);
	else if (status.role == SLAVE)
	{
		handle_slave();
		status.countdown = 3;
	}
}


// Triggers when recieving info slave (Here we dont need the specific byte send as it only triggers when slave press a button)
void on_slave_info(uint8_t info)
{
	(void) info;
	if (status.waiting && !status.started)
	{
		status.waiting = 0;
		status.started = 1;
		start_game();
	}
	else if (status.started)
	{
		status.waiting = 0;
		status.sl_button_pressed = 1;
		resolve_game();
	}
}