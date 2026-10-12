#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BIT_MASK(bit) ((1 << bit))
#define SET_BIT(REGISTER, bit) (REGISTER |= BIT_MASK(bit))
#define CLEAR_BIT(REGISTER, bit) (REGISTER &= ~BIT_MASK(bit))
#define LED_MASK_PORTB (BIT_MASK(PORTB0) | BIT_MASK(PORTB1) | BIT_MASK(PORTB2) | BIT_MASK(PORTB4))
#define LOOP while(1)
#define USV 0 // Username validity
#define PSV 1 // Password validity
#define CS 2 // Check state , 1 means it has checked username and are checking password
#define CONNECTED 3
#define BACKSPACE 127

typedef struct s_state
{
    char    *current_username_address;
    char    *current_password_address;
    char    *username_last_valid_addr;
    char    *password_last_valid_addr;
    uint8_t validity;
}           t_state;

void    uart_printstr(const char* str);
void    uart_tx(char c);
void    manage_backspace();
void    manage_password_validity(char c);
void    manage_username_validity(char c);
uint8_t ft_strlen(char *str);