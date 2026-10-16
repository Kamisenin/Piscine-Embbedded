#include "eeprom.h"

int main(void)
{
	uart_init();
	eeprom_hexdump(0x00, 0x3FF);
}	