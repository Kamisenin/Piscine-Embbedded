#pragma once

#include <avr/io.h>

#define CTC_1 _BV(WGM12)
#define CTC_2 _BV(WGM13)

#define PRESCALE_1 _BV(CS10)
#define PRESCALE_8 _BV(CS11)
#define PRESCALE_64 (_BV(CS11) | _BV(CS10))
#define PRESCALE_256 _BV(CS12)
#define PRESCALE_1024 (_BV(CS12) | _BV(CS11))