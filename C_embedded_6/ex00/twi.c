#include "twi.h"

/*
*	TWBR = TWI bit rate Register, select the division factor for the bit rate generator which generate the SCL clock frequency
*	F_CPU / (16 + 2 * TWBR * prescaler) gives us the Communication Frequency in Hz. (22.5.2 p222) 
*	To Compute the TWBR we need to reverse the Formula which gives us : (F_CPU / Frequency - 16) / 2 * prescaler
*	If we simplify the formula to not use unecessary division we end up with 
* (F_CPU - 16 * Frequency) / 2 * Frequency * prescaler, which, with a prescaler of 1 and a frequency of 100khz
* gives us (16 000 000 - 16 * 100 000) / 2 * 100 000 * 1 = 79.2 which round is 79

 */
void	i2c_init(void)
{
	TWCR |= TW_ACK;
	// TWSR = TWI Status Register. It stores TWI communication status code and prescaler
	TWSR &= ~(_BV(TWPS1) | _BV(TWPS0)); // clear the prescaler to 1 (posiblity : 1, 4, 16 and 64, none give round number so we take the nearest which is 1) technically it is already set to 0 but for security reason we clear it to 0
	TWBR = 72; // Bit rate value to reach a 100kHz
}


void	wait_transmission()
{
	while (!(TWCR & _BV(TWINT)));
}

void	i2c_start(void)
{
	TWCR = TW_ACK | _BV(TWSTA);
}

void i2c_stop(void)
{
	TWCR = TW_NACK | _BV(TWSTO);
}