#include "network_node.h"

extern uint16_t	slots_address[];
extern uint8_t csi;

/*
 * Writes to eeprom node raw values from start to end (excluded)
 * If writing fails, relocates the node until writing works or no node is left
 */
_Bool   write_node_bytes(t_node *node, uint8_t start_byte, uint8_t end_byte)
{
    while (eeprom_write_block(&node->raw[start_byte], slots_address[csi] + start_byte, end_byte - start_byte))
	{
		if (data_fallback(node))
			return (1);
	}
    return (0);
}

/*
 * handle error message and relocation
 */
_Bool	data_fallback(t_node *out)
{
	uart_printstr("CRITICAL: Data corruption detected!\r\n");
	if (node_relocate(out))
	{
		uart_printstr("CRITICAL EEPROM FAILURE\r\n");
		return (1);
	}
	return (0);
}


_Bool	is_data_corrupt(t_node node)
{
	return (node.checksum != crc32(node.data.raw, sizeof(t_payload)));
}

_Bool str_to_i16(int16_t *out, const unsigned char *str)
{
    uint32_t value = 0;
    uint32_t limit = INT16_MAX;
    _Bool     negative = 0;

    if (!out || !str)
        return (0);
    if (*str == '-')
    {
        negative = 1;
        limit = (uint32_t)INT16_MAX + 1;
        str++;
    }
    else if (*str == '+')
        str++;
    if (*str == '\0')
        return (0);
    while (*str)
    {
        if (*str < '0' || *str > '9')
            return (0);

        uint32_t digit = *str - '0';

        if (value > (limit - digit) / 10)
            return (0);

        value = value * 10 + digit;
        str++;
    }
    *out = negative ? (int16_t)(-(int32_t)value) : (int16_t)value;
    return (1);
}


_Bool	str_to_u32(uint32_t *out, const unsigned char *str)
{
    uint32_t value = 0;

    if (!out || !str || *str == '\0')
        return (0);

    while (*str)
    {
        if (*str < '0' || *str > '9')
            return (0);

        uint32_t digit = *str - '0';

        if (value > (UINT32_MAX - digit) / 10)
            return (0);

        value = value * 10 + digit;
        str++;
    }
    *out = value;
    return (1);
}