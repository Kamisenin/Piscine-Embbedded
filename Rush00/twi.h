#pragma once

#include <avr/io.h>
#include "bit_operation.h"
#include "rgb.h"
#include <util/twi.h>
#include "utils.h"

#define TRANSMIT_TWI(data) (TWDR = data)

#define TW_SEND			(_BV(TWINT) | _BV(TWEN) | _BV(TWIE) | _BV(TWEA)) // Set TWEN bit to enable communication, and TWIE to enable interrupts, TWEA acknowledge response
#define TW_LAST			(_BV(TWINT) | _BV(TWEN) | _BV(TWIE)) // Dont ACK the reception to stop slave from sending information

#define SLAVE_ADDRESS   0x10
#define MASTER_ADDRESS  0x11

#define READ_SLAVE_MSK	(SLAVE_ADDRESS << 1) | TW_READ
#define WRITE_SLAVE_MSK	(SLAVE_ADDRESS << 1) | TW_WRITE

#define MASTER 1
#define SLAVE 0

#define POLL_FOREVER 0

void	twi_init();
void	write_mode(void);
void	start_polling(uint32_t max_tries);
void	reset_polling();
void	handle_poll(void);
void	handle_master_tx(uint8_t data);

void 	on_slave_info(uint8_t info); // define this function in another file to use it when slave gives you infos through poll
void 	on_poll_timeout(void); // define this function in another file to manage poll timeouts
void	on_slave_disconnect(void); // define this function in another file to manage diconnections
void	on_slave_not_recieve(void); // define this function in another file to manage slave not recieving data
void	on_master_bus_error(void); //  define this function in another file to manage bus error on Master side
/*
 * Triggers when Master has finished sending data to the bus
 * define this event function in to manage your datas
 */
void	on_master_data_sent(void);
uint8_t	on_poll_idle(void);