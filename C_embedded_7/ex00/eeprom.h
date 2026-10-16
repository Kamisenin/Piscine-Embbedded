#pragma once

#include <avr/eeprom.h>
#include "uart.h"
#include "bit_operation.h"

uint8_t eeprom_read(uint16_t address);
void    eeprom_hexdump(const uint16_t start, const uint16_t stop);
void    eeprom_busy();