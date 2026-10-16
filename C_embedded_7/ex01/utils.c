#include "utils.h"

uint16_t	ahtoi(unsigned char *str, uint8_t *i)
{
	uint16_t n = 0;
	while (str[*i])
	{
		if (str[*i] >= 'A' && str[*i] <= 'F')
			n = n * 16 + str[*i] - 'A' + 10;
		else if (str[*i] >= 'a' && str[*i] <= 'f')
			n = n * 16 + str[*i] - 'a' + 10;
		else if (str[*i] >= '0' && str[*i] <= '9')
			n = n * 16 + str[*i] - '0';
		else
			return (n);
		(*i)++; 
	}
	return (n);
}

_Bool	is_prompt_valid(unsigned char *str)
{
	uint8_t i;
	uint8_t j;

	for (i = 0; i < 3; i++)
	{
		if (str[i] == ' ')
			break;
		if (!is_hex_digit(str[i]))
			return (0);
	}
	if (i == 0 || str[i] != ' ')
		return (0);
	for (j = i + 1; j < i + 3; j++)
	{
		if (!is_hex_digit(str[j]))
			return (0);
	}
	if (str[j] != '\0')
		return (0);
	return (1);
}


_Bool	is_hex_digit(unsigned char c)
{
	return ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') 
		|| (c >= '0' && c <= '9'));
}

_Bool	is_printable(unsigned char c)
{
	return (c > 31 && c < 127);
}


void	print_red(char *str)
{
	uart_printstr(RED_COLOR);
	uart_printstr(str);
	uart_printstr(CLEAR_COLOR);
}