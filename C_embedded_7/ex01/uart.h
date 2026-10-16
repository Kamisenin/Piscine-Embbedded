#pragma once

#include "bit_operation.h"

#define BACKSPACE 127

uint8_t	uart_rx(void);
void	uart_tx(char c);
void	uart_init();
void	uart_printstr(const char* str);
void	uart_print_u_str(const unsigned char* str);
void    uart_putnbr_base(uint16_t n, uint8_t base_length);
void    uart_putnbr_base_width(uint16_t n, uint8_t base_len, int width, unsigned char pad);