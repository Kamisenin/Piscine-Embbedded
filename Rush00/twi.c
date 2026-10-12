#include "twi.h"

volatile uint8_t	polling = 0;
volatile uint32_t 	poll_left = 0;
volatile uint8_t  	poll_infinite = 0;
volatile uint8_t  	poll_timed_out = 0;
volatile uint8_t	rx_status = 0;


/*
*	TWBR = TWI bit rate Register, select the division factor for the bit rate generator which generate the SCL clock frequency
*	F_CPU / (16 + 2 * TWBR * prescaler) gives us the Communication Frequency in Hz. (22.5.2 p222) 
*	To Compute the TWBR we need to reverse the Formula which gives us : (F_CPU / Frequency - 16) / 2 * prescaler
*	If we simplify the formula to not use unecessary division we end up with 
* (F_CPU - 16 * Frequency) / 2 * Frequency * prescaler, which, with a prescaler of 1 and a frequency of 100khz
* gives us (16 000 000 - 16 * 100 000) / 2 * 100 000 * 1 = 79.2 which round is 79

 */
void twi_init()
{
	TWCR |= TW_SEND;
	// TWSR = TWI Status Register. It stores TWI communication status code and prescaler
	TWSR &= ~(_BV(TWPS1) | _BV(TWPS0)); // clear the prescaler to 1 (posiblity : 1, 4, 16 and 64, none give round number so we take the nearest which is 1) technically it is already set to 0 but for security reason we clear it to 0
	TWBR = 79;
	// TWAR = TWI Adress Register
	TWAR = (SLAVE_ADDRESS << 1); // first bit is used for broadcast so skip this one, put SLAVE_ADDRESS Until Master is decided
}


void start_polling(uint32_t max_tries)
{
	poll_timed_out = 0;
    polling = 1;
    poll_infinite = (max_tries == POLL_FOREVER);
    poll_left = max_tries;
	TWCR = TW_SEND | _BV(TWSTA);
}

static uint8_t poll_again(void)
{
	if (poll_infinite)
		return 1;
	return (--poll_left != 0);
}

static void stop_polling(uint8_t timed_out)
{
    polling = 0;
    poll_timed_out = timed_out;
    TWCR = TW_SEND | _BV(TWSTO);
}

void	reset_polling()
{
	polling = 0;
	poll_left = 0;
	poll_infinite = 0;
	poll_timed_out = 0;
}

/*
 *	MR = Master Reciever
 *	MS = Master Transmitter
 *	ST = Slave Transmitter
 *	SR = Slave Reciever
 */
void handle_poll(void)
{
	switch (TW_STATUS)
    {
		case TW_START: // When the Board gets the BUS and is switched to Master
		case TW_REP_START: //Rep start happens when you start without closing the bus
			TWDR = (SLAVE_ADDRESS << 1) | TW_READ;
			TWCR = TW_SEND;
			break;

		case TW_MR_SLA_ACK: // when Recieving a ACK from Slave
			TWCR = TW_LAST; // Only need 1 byte so we set this one to last and stop using bus
			break;

		case TW_MR_DATA_NACK: // after recieving byte get byte, check if its empty, poll again
			rx_status = TWDR;
			if (rx_status != 0)
			{
				stop_polling(0);
				on_slave_info(rx_status);
			}
			else if (on_poll_idle());
			else if (poll_again())
				TWCR = TW_SEND | _BV(TWSTA);    // poll again
			else
			{
				stop_polling(1);
				on_poll_timeout();
			}
			break;

		case TW_MR_SLA_NACK: // When recieving a NACK from Slave at initialisation
			on_slave_disconnect();
			break;

		case TW_BUS_ERROR:
    		on_master_bus_error();
    		break;

		default:
			TWCR = TW_SEND;
			break;
    }
}

void	handle_master_tx(uint8_t data)
{
	switch (TW_STATUS)
	{
		case TW_START:
		case TW_REP_START: // on start send slave on write address
			TWDR = WRITE_SLAVE_MSK;
			TWCR = TW_SEND;
			break;
		
		case TW_MT_SLA_ACK: // Slave responded to address with ACK send data
			TWDR = data;
			TWCR = TW_SEND;
			break;

		case TW_MT_DATA_ACK: // data sent to SLAVE, stopping BUS
			on_master_data_sent();
			break;

		case TW_MT_SLA_NACK: // no response to address, slave is disconnected
			on_slave_disconnect();
			break;

		case TW_MT_DATA_NACK: // Slave responded to address but refused acknowledgment of data (TWEA not enabled or disconnection during data transmission)
			on_slave_not_recieve();
			break;

		case TW_BUS_ERROR:
			on_master_bus_error();
			break;
		
		default:
    		TWCR = TW_SEND;
    		break;
			
	}
}

void	write_mode(void)
{
	TWCR = TW_SEND | _BV(TWSTA);
}

__attribute__((weak))
void 	on_slave_info(uint8_t info) { (void) info; }

__attribute__((weak))
void	on_poll_timeout(void) {}

__attribute__((weak))
void	on_slave_disconnect(void) {}

__attribute__((weak))
void	on_slave_not_recieve(void) {}

__attribute__((weak))
void	on_master_bus_error(void) {}

__attribute__((weak))
void	on_master_data_sent(void) {}

__attribute__((weak))
uint8_t on_poll_idle(void) { return 0; }