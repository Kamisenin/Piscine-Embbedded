#pragma once

#include "../eeprom.h"

#define CRC32C_DIVIDER 0x82F63B78 // bit inverse of Castagnoli's polynome of 0x1EDC6F41, since we want to use little endian

#define NODE_MAGIC	18
#define SLOT_COUNT	4
#define NO_SLOT		0xFF

typedef enum {
	NODE_UNCONFIGURED,
	NODE_CORRUPTED,
	NODE_VALID
} t_node_state;

typedef enum {
    SLOT_EMPTY,
    SLOT_BAD,
    SLOT_VALID
} t_slot_status;

typedef union __attribute__((packed)) u_payload {
	struct __attribute__((packed)) {
		uint32_t	node_id;
		int16_t		priority;
		uint8_t		tag[33];
	};
	uint8_t	raw[39];
} t_payload;

typedef union __attribute__((packed)) u_node {
	struct __attribute__((packed)) {
		uint8_t		magic;
		t_payload	data;
		uint16_t	version;
		uint32_t	checksum;
	};
	uint8_t raw[46];
} t_node;

// -----Node commands------
_Bool			node_relocate(t_node *node_ref);
t_node_state	node_init(t_node *out);
_Bool			node_write(t_node *node, uint8_t start_byte, uint8_t end_byte);
t_node			node_read_memory(uint8_t index);
t_slot_status	slot_status(uint8_t i, t_node *out);

// -----Commands------
void	status(t_node *node);
void	set_id(unsigned char *cli, t_node *node);
void	set_prio(unsigned char *cli, t_node *node);
void	set_tag(unsigned char *cli, t_node *node);
void	factory_reset(t_node *out);

// -----Node utilitary------
_Bool   write_node_bytes(t_node *node, uint8_t start_byte, uint8_t end_byte);
_Bool	is_data_corrupt(t_node node);
_Bool	data_fallback(t_node *out);
_Bool	str_to_i16(int16_t *out, const unsigned char *str);
_Bool	str_to_u32(uint32_t *out, const unsigned char *str);
_Bool	node_set_default(t_node *node);

uint32_t crc32(const uint8_t *d, uint8_t n);