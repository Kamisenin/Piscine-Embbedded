#pragma once

#include <avr/io.h>

#define SET_BIT(register, bit) (register |= _BV(bit))
#define CLEAR_BIT(register, bit) (register &= ~_BV(bit))
#define CLEAR_LED PORTD &= ~(_BV(PORTD3) | _BV(PORTD5) | _BV(PORTD6))