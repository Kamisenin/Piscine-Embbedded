#include "utils.h"


extern t_state state;
extern const char username[];
extern const char password[];


void uart_tx(char c)
{
    while ( !( UCSR0A & (1<<UDRE0)) ); // wait until info has been sent

    UDR0 = c; // send the data to the register
}

void uart_printstr(const char* str)
{
    while (*str)
    {
        uart_tx(*str);
        str++;
    }
}


uint8_t ft_strlen(char *str)
{
    uint8_t i = 0;

    while (str[i++]);
    
    return (i);
}

void manage_username_validity(char c)
{
    if (state.validity & BIT_MASK(USV) || state.current_username_address == username)
    {
        if (*state.current_username_address ==  '\0' || *state.current_username_address != c)
            CLEAR_BIT(state.validity, USV);
        else
        {
            SET_BIT(state.validity, USV);
            state.username_last_valid_addr = state.current_username_address;
        }
    }
    state.current_username_address++;
}

void manage_password_validity(char c)
{
    if (state.validity & BIT_MASK(PSV) || state.current_password_address == password)
    {
        if (*state.current_password_address ==  '\0' || *state.current_password_address != c)
            CLEAR_BIT(state.validity, PSV);
        else
        {
            SET_BIT(state.validity, USV);
            state.password_last_valid_addr = state.current_password_address;
        }
    }
    state.current_password_address++;
}

void manage_backspace()
{
    if (state.validity & BIT_MASK(CS))
    {
        if (state.current_password_address == password)
            return ;
        state.current_password_address--;
        if (state.current_password_address == state.password_last_valid_addr)
            SET_BIT(state.validity, PSV);
    }
    else
    {
        if (state.current_username_address == username)
            return ;
        state.current_username_address--;
        if (state.current_username_address == state.username_last_valid_addr)
            SET_BIT(state.validity, USV);
    }
    uart_printstr("\b \b");
}