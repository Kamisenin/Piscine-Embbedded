#pragma once

#include <avr/io.h>

#define SET_BIT(register, bit) (register |= _BV(bit))
#define CLEAR_BIT(register, bit) (register &= ~_BV(bit))