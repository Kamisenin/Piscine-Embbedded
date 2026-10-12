#include <avr/io.h>

void inject_bit(uint8_t source, volatile uint8_t *destination, uint8_t pos_source, uint8_t pos_dest)
{
	uint8_t bit = (source >> pos_source) & 1;

	*destination = ((*destination) & ~(1 << pos_dest)) | (bit << pos_dest);
}

bool is_button_pressed(uint8_t registry, uint8_t button)
{
	return (!(registry & (1 << button)));
}