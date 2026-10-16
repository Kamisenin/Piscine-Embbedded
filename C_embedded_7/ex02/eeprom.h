#pragma once

#include <avr/io.h>
#include "uart.h"
#include "bit_operation.h"

uint8_t eeprom_read(const uint16_t address);
void    eeprom_write(const uint16_t address, const unsigned char data);
_Bool	eeprom_safe_write(const uint16_t address, const uint8_t data);
void    eeprom_hexdump(const uint16_t start, const uint16_t stop);
void	eeprom_read_block(const void *dst, const uint16_t address, const uint8_t size);
_Bool	eeprom_write_block(const void *src, const uint16_t address, const uint8_t size);
void    eeprom_busy();
char	eeprom_print_hex(uint16_t address);