#pragma once
#include <avr/io.h>

uint8_t	ft_atoi_n_base(const char *nptr, char *n,  char *base);
uint8_t	ft_strlen(const char *s);
_Bool   ft_is_in_str(char c, char *str);