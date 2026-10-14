#pragma once

bool is_button_pressed(uint8_t registry, uint8_t button);

void inject_bit(uint8_t source, volatile uint8_t *destination, uint8_t pos_source, uint8_t pos_dest);