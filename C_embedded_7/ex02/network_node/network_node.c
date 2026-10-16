#include "network_node.h"


uint16_t	slots_address[] = {
	0x030, 0x070, 0x100, 0x300
};

#ifdef DEBUG
static void	debug_factory_reset();
#endif

uint8_t csi = NO_SLOT; // current slot index

t_node_state node_init(t_node *out)
{
	uint8_t	first_empty = NO_SLOT;
	t_node	best = {0};
	t_node	tmp;

	#ifdef DEBUG
	debug_factory_reset();
	#endif

	csi = NO_SLOT;
	for (uint8_t i = 0; i < SLOT_COUNT; i++)
	{
		t_slot_status st = slot_status(i, &tmp);

		if (st == SLOT_EMPTY && first_empty == NO_SLOT)
			first_empty = i;
		else if (st == SLOT_VALID && (!best.magic || tmp.version > best.version))
		{
			best = tmp;
			csi = i;
		}
	}
	if (best.magic)
	{
		*out = best;
		return (NODE_VALID);
	}
	csi = first_empty;
	return (first_empty == NO_SLOT ? NODE_CORRUPTED : NODE_UNCONFIGURED);
}

t_slot_status slot_status(uint8_t i, t_node *out)
{
	if (eeprom_read(slots_address[i]) != NODE_MAGIC)
		return (SLOT_EMPTY);

	*out = node_read_memory(i);
	if (is_data_corrupt(*out))
		return (SLOT_BAD);
	return (SLOT_VALID);
}

_Bool	node_relocate(t_node *node_ref)
{
	uint8_t i;

	for (i = 0; i < SLOT_COUNT; i++)
	{
		if (i == csi)
			continue ;
		uart_printstr("Relocating config to slot ");
		uart_putnbr_base(i + 1, 10);
		uart_printstr("... ");
		if (eeprom_read(slots_address[i]) == NODE_MAGIC)
		{
			t_node node = node_read_memory(i);
			if (is_data_corrupt(node))
			{
				uart_printstr("Failure\r\n");
				continue;
			}
			if (node.version >= node_ref->version)
			{
				*node_ref = node;
				break;
			}
		}
		uint8_t temp = csi;
		csi = i;
		if (eeprom_write_block(node_ref->raw, slots_address[i], sizeof(t_node)))
		{
			uart_printstr("Failure\r\n");
			csi = temp;
			continue;
		}
		uart_printstr("Success\r\n");
		break;
	}
	if (i < SLOT_COUNT)
		csi = i;
	return (i == SLOT_COUNT);
}

/*
 * Writes to eeprom node raw values from start to end (excluded)
 * and Updates checksum and version control.
 * If writing fails, relocates the node until writing works or no node is left
 */
_Bool	node_write(t_node *node, uint8_t start_byte, uint8_t end_byte)
{
	node->version += 1;
	node->checksum = crc32(node->data.raw, sizeof(t_payload));
	
	if (write_node_bytes(node, start_byte, end_byte))
		return (1);

	// write version & checksum bits[40:46]
	if (write_node_bytes(node, 40, sizeof(t_node)))
		return (1);
	return (0);
}


t_node	node_read_memory(uint8_t index)
{
	t_node node = {0};

	eeprom_read_block(node.raw, slots_address[index], sizeof(t_node));

	return (node);
}

_Bool	node_set_default(t_node *node)
{
	static const char undefined[11] = "Undefined";
	int i;
	for (i = 0; i < 11; i++)
	{
		node->data.tag[i] = undefined[i];
	}
	while (i < 33)
	{
		node->data.tag[i] = '\0';
		i++;
	}
	node->magic = NODE_MAGIC;
	node->version += 1;
	node->checksum = crc32(node->data.raw, sizeof(t_payload));
	if (write_node_bytes(node, 0, sizeof(t_node)))
		return (1);
	return (0);
}

uint32_t crc32(const uint8_t *d, uint8_t n)
{
	uint32_t c = 0xFFFFFFFF;
	while (n--) {
		c ^= *d++;
		for (int i = 0; i < 8; i++)
			c = (c >> 1) ^ (CRC32C_DIVIDER & -(c & 1));
	}
	return ~c;
}

#ifdef DEBUG
void	debug_factory_reset()
{
	t_node zero = {0};

	for (uint8_t i = 0; i < SLOT_COUNT; i++)
		eeprom_write_block(zero.raw, slots_address[i], sizeof(t_node));
	csi = NO_SLOT;
}
#endif