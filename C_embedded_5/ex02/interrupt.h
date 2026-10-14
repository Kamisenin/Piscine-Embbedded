#pragma once

#define NO_INTERRUP __signal__
#define USED __used__
#define VISIBLE __externally_visible__

#define INTERRUPTION 7

#define CLEAR_INTERRUPT SREG &= ~_BV(INTERRUPTION)
#define SET_INTERRUPT SREG |= _BV(INTERRUPTION)