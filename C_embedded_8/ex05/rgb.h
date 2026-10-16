#include <avr/io.h>


#define RED 255UL
#define GREEN (255UL << 8) 
#define BLUE (255UL << 16)
#define BRIGHTNESS (10UL << 24)  // Brigthness of 20%
#define MASK_BR(color) (BRIGHTNESS | (color))
#define HEX "0123456789ABCDEF"

void		init_rgp_spi();
void		color(uint8_t brightness, const uint8_t blue, const uint8_t green, const uint8_t red);
void		display_color(const volatile uint32_t *colors, const uint8_t n);
uint32_t	wheel(uint32_t pos);