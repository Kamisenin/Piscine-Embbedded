#pragma once

#include <avr/io.h>
#include <util/twi.h>

#define TRANSMIT_TWI(data) (TWDR = data)

#define TW_ACK			(_BV(TWINT) | _BV(TWEN) | _BV(TWEA)) // Set TWEN bit to enable communication, TWEA acknowledge response
#define TW_NACK			(_BV(TWINT) | _BV(TWEN)) // Dont ACK the reception to stop slave from sending information

#define READ_SLAVE_MSK	(SLAVE_ADDRESS << 1) | TW_READ
#define WRITE_SLAVE_MSK	(SLAVE_ADDRESS << 1) | TW_WRITE

#define MASTER 1
#define SLAVE 0

#define POLL_FOREVER 0


#ifndef PROGRAM_ADDR
# define PROGRAM_ADDR 0x12
#endif

void	i2c_init(void);
_Bool	i2c_init_read(uint8_t address);
_Bool	i2c_init_write(uint8_t address);


void    i2c_start(void);
void    i2c_stop(void);
void	i2c_read(void);
void	i2c_read_last(void);
void    i2c_write(unsigned char data);
uint8_t	i2c_get_read_last(void);