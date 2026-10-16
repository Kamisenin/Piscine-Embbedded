#pragma once

#include <avr/io.h>
#include "uart.h"

uint16_t	ahtoi(unsigned char *str, uint8_t *i);
_Bool		is_prompt_valid(unsigned char *str);
_Bool		is_hex_digit(unsigned char c);
_Bool		is_printable(unsigned char c);
int8_t	    ft_strncmp(const unsigned char *s1, const char *s2, uint8_t n);