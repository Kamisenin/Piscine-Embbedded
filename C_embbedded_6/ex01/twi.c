#include "twi.h"
#include "uart.h"



static void	wait_transmission();
static void	print_hex_value(unsigned char c);
static void	transmit_address(uint8_t address);


/*	
 *	//---------------------\\
 *	||--------Init---------||
 *	\\---------------------//
 */

void	i2c_init(void)
{
	TWCR |= TW_ACK;
	// TWSR = TWI Status Register. It stores TWI communication status code and prescaler
	TWSR &= ~(_BV(TWPS1) | _BV(TWPS0)); // clear the prescaler to 1
	TWBR = 72; // Bit rate value to reach a 100kHz
	TWAR = PROGRAM_ADDR << 1; // set program i2c address
}

_Bool	i2c_init_write(uint8_t address)
{
	i2c_start();
	transmit_address((address << 1) | TW_WRITE); // transmit address in write mode
	if (TW_STATUS != TW_MT_SLA_ACK)
		return (1); // wrong address or inreachable slave
	return (0);
}

_Bool	i2c_init_read(uint8_t address)
{
	i2c_start();
	transmit_address((address << 1) | TW_READ); // transmit address in read mode
	if (TW_STATUS != TW_MR_SLA_ACK)
		return (1); // wrong address or inreachable slave
	return (0);
}


/*	
 *	//---------------------\\
 *	||----I2c Operation----||
 *	\\---------------------//
 */

void	i2c_start()
{
	TWCR = TW_ACK | _BV(TWSTA);
	wait_transmission();
}

void	i2c_stop(void)
{
	TWCR = TW_NACK | _BV(TWSTO);
}

void	i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = TW_ACK;
	wait_transmission();
}

void	i2c_read(void)
{
	TWCR = TW_ACK;
	wait_transmission();
	print_hex_value(TWDR);
}

void	i2c_read_last(void)
{
	TWCR = TW_NACK;
	wait_transmission();
	print_hex_value(TWDR);
}


uint8_t	i2c_get_read_last(void)
{
	TWCR = TW_NACK;
	wait_transmission();
	return (TWDR);
}


/*	
 *	//---------------------\\
 *	||-----Utilitaries-----||
 *	\\---------------------//
 */

void	transmit_address(uint8_t address)
{
	TWDR = address;
	TWCR = TW_ACK;

	wait_transmission();
}

void print_hex_value(unsigned char c)
{
	static const char base[17] = "0123456789ABCDEF";
	
	if (c >= 16)
	{
		print_hex_value(c / 16);
		print_hex_value(c % 16);
	}
	else
		uart_tx(base[c]);
}

void	wait_transmission()
{
	while (!(TWCR & _BV(TWINT)));
}
