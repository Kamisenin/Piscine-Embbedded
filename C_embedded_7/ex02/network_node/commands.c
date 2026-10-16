#include "network_node.h"


extern uint16_t	slots_address[];
extern uint8_t csi;

/*
 * STATUS
 * SET_ID
 * SET_PRIO
 * SET_TAG
 * FACTORY_RESET
*/

static void		print_node_data(t_node node);
static uint8_t	is_valid_tag(unsigned char *str);

void	status(t_node *node)
{
	switch (slot_status(csi, node))
	{
		case NODE_UNCONFIGURED:
			uart_printstr("Node unconfigured\r\n");
			break;
		
		case NODE_CORRUPTED:
			uart_printstr("CRITICAL: Data corruption detected!\r\n");
			break;
		
		default :
			print_node_data(*node);
			break;
	}
}

void	set_id(unsigned char *cli, t_node *node)
{
	uint32_t	id = 0;

	if (!cli[0] || !str_to_u32(&id, cli))
	{
		uart_printstr("Invalid Argument\r\n");
		return ;
	}
	switch (slot_status(csi, node))
	{
		case NODE_UNCONFIGURED:
			if (node_set_default(node))
				return ;
			break;
		
		case NODE_CORRUPTED:
			if (data_fallback(node))
				return ;
			break;
		default:
			break;
	}
	node->data.node_id = id;
	// write node id bytes [1:4]
	node_write(node, 1, 5);
	#ifdef DEBUG
		eeprom_write(slots_address[csi] + sizeof(t_node) - 2, 18);
	#endif
}

void	set_prio(unsigned char *cli, t_node *node)
{
	int16_t prio = 0;

	if (!cli[0] || !str_to_i16(&prio, cli))
	{
		uart_printstr("Invalid Argument\r\n");
		return ;
	}
	switch (slot_status(csi, node))
	{
		case NODE_UNCONFIGURED:
			if (node_set_default(node))
				return ;
			break;
		
		case NODE_CORRUPTED:
			if (data_fallback(node))
				return ;
			break;
		default:
			break;
	}
	node->data.priority = prio;
	// write priority bytes [5:7]
	node_write(node, 5, 7);
}

void	set_tag(unsigned char *cli, t_node *node)
{
	int i = 0;
	uint8_t max = 0;
	if (!(max = is_valid_tag(cli)))
	{
		uart_printstr("Invalid Argument\r\n");
		return ;
	}
	switch (slot_status(csi, node))
	{
		case NODE_UNCONFIGURED:
			if (node_set_default(node))
				return ;
			break;
		
		case NODE_CORRUPTED:
			if (data_fallback(node))
				return ;
			break;
		default:
			break;
	}

	while (i + 1 < max)
	{
		node->data.tag[i] = cli[i + 1];
		i++;
	}
	while (i < 33)
	{
		node->data.tag[i] = '\0';
		i++;
	}
	// write data bytes [7:40]
	node_write(node, 7, 40);
}


void	factory_reset(t_node *out)
{
	if (csi == NO_SLOT)
		return ;
	t_node node = {0};

	*out = node;

	eeprom_write_block(node.raw, slots_address[csi], sizeof(t_node));
}


void		print_node_data(t_node node)
{
	uart_printstr("Node ID: ");
	uart_putnbr_base(node.data.node_id, 10);
	uart_printstr("\r\nPriority: ");
	uart_putnbr_signed(node.data.priority);
	uart_printstr("\r\nSlot: ");	
	uart_putnbr_base(csi + 1, 10);
	uart_printstr("\r\nTag: \"");
	uart_print_u_str(node.data.tag);
	uart_printstr("\"\r\n");
}


uint8_t	is_valid_tag(unsigned char *str)
{
	uint8_t i = 1;

	if (*str != '"')
		return 0;

	while ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')
		|| str[i] == '-' || str[i] == '_' || (str[i] >= '0' && str[i] <= '9'))
	{
		i++;
	}
	if (str[i] == '"' && str[i + 1] == '\0')
		return (i);
	return (0);

}