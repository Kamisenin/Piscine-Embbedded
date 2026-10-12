#include <avr/io.h>

uint8_t	ft_strlen(const char *s)
{
	uint8_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

_Bool ft_is_in_str(char c, char *str)
{
    while (*str)
    {
        if (c == *str)
            return (1);
        str++;
    }
    return (0);
}

static int  get_index(char c, const char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (i);
}

uint8_t	ft_atoi_n_base(const char *nptr, char *n,  char *base)
{
	uint8_t		base_len;
	uint8_t		index;
	uint8_t 	result;

	result = 0;
	base_len = ft_strlen(base);
	index = get_index(*nptr, base);
	while (index < base_len && nptr != n)
	{
		result = result * base_len + index;
		nptr++;
		index = get_index(*nptr, base);
	}
	return (result);
}