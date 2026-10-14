#pragma once

#include <avr/io.h>

#define HEX "0123456789ABCDEF"

typedef struct s_rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}           t_rgb;

void    wheel(uint8_t pos);
void    set_rgb(t_rgb rgb);
void    init_rgb();
t_rgb   get_rgb_from_hex(char *hex);
void    set_luminosity(uint8_t l);