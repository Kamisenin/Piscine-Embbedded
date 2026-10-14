#pragma once

#include <avr/io.h>

void wheel(uint8_t pos);
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
void init_rgb();
void set_luminosity(uint8_t l);