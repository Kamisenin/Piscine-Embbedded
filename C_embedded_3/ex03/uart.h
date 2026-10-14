#pragma once

#include <avr/io.h>

uint8_t	uart_rx(void);
void	uart_tx(char c);
void	uart_init();
void	uart_printstr(const char* str);