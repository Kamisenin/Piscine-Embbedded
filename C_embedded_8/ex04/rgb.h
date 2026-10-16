#include <avr/io.h>

void        init_rgp_spi();
void    	color(uint8_t brightness, const uint8_t blue, const uint8_t green, const uint8_t red);
void	    display_color(const uint32_t *colors, const uint8_t n);
uint32_t    conver_hex_to_rgb(unsigned char *str);
void        set_rainbow();
void        stop_rainbow();
_Bool	    get_rainbow();
void        wheel(uint8_t pos);