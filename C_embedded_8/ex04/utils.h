#pragma once

#include <avr/io.h>

uint8_t	ft_atoi_n_base(const unsigned char *nptr, unsigned char *n,  char *base);
uint8_t	ft_strlen(const char *s);
int8_t	ft_strncmp(const unsigned char *s1, const char *s2, uint8_t n);
_Bool   ft_is_in_str(char c, char *str);
_Bool	is_printable(unsigned char c);
_Bool	is_hex_digit(unsigned char c);