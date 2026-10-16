#pragma once

#include <avr/io.h>
#include "uart.h"

#define CLEAR_COLOR "\001\e[0m\002"
#define RED_COLOR "\001\e[1m\e[38;5;196m\002"

uint16_t	ahtoi(unsigned char *str, uint8_t *i);
_Bool		is_prompt_valid(unsigned char *str);
_Bool		is_hex_digit(unsigned char c);
_Bool		is_printable(unsigned char c);
void		print_red(char *str);