#pragma once

#include "twi.h"
#include "interrupt.h"
#include <util/delay.h>

typedef struct s_state
{
	uint8_t role : 1;
	uint8_t waiting : 1; // Wait for the slave/master setup, at 0 if connection not setup
	uint8_t started : 1;
	uint8_t countdown : 3;
	uint8_t	sl_button_pressed : 1;
	uint8_t	mt_button_pressed : 1;
}			t_state;

void	handle_slave_led_info(uint8_t led);
void    start_game(void);
void    stop_game(void);
void	resolve_game(void);
void	start_slave_watchdog(void);