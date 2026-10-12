#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "rgb.h"

int main()
{
	init_rgb();
	uint8_t i = 0;
	set_luminosity(50);
	while (1)
	{
		wheel(i);
		i++;
		_delay_ms(20);
	}
}