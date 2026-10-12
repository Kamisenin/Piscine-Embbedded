#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

#include "interrupt.h"
#include "bit_operation.h"
#include <util/delay.h>


#define INJECT(source, pos_source, register, bit) (register = (register & ~_BV(bit)) | ((source >> pos_source) & 1) <<  bit)

volatile uint8_t	sw1_state = 1;
volatile uint8_t	sw2_state = 1;
volatile uint8_t	value = 0;


int	main()
{
	CLEAR_INTERRUPT;
      	DDRB |= _BV(DDB0) | _BV(DDB1) | _BV(DDB2)| _BV(DDB4);	
	// EICRA = External Interrupt Control Register A (13.2.1 p.80)
	
	/* INTERRUPT ACTIVATION*/
	// EICRA |= _BV(ISC01); // ? activate interrupt on falling edge (Push button) (13-2 p.80)
	// EIMSK |= _BV(INT0); // ? activate external interrupt for SW1

	/*
	*	PCICR (Pin change interrupt control register) allows to flag certain pins to create interruption on current change.
	*	For better code readablity, we will not use INT0, 
	*	instead triggering both buttons using this interrupt
	*	There is 3 possible interrupt, PCIE0, PCIE1 and PCIE2.
	*	after activating them, you have to put enable the PIN's interruption you want
	*	Using PCMSKx (PIN change Mask register) we can flag the pins we want, finding the right value on section 14
	*	(for us PORTD's pins are at section 14.3.3 page 97), SW2 is PCINT20, SW1 PCINT18
	*/
	PCMSK2 |= _BV(PCINT20) | _BV(PCINT18); // activate interrupt of SW2 and SW1
	PCICR |= _BV(PCIE2); // allow external interrupt from PCINT2 vector
	SET_INTERRUPT;
	while (1);
}

void display_value()
{
	INJECT(value, 0, PORTB, PORTB0);
	INJECT(value, 1, PORTB, PORTB1);
	INJECT(value, 2, PORTB, PORTB2);
	INJECT(value, 3, PORTB, PORTB4);
}

void	PCINT2_vect(void) __attribute__ ((NO_INTERRUP, USED, VISIBLE));

void	PCINT2_vect(void)
{
	_delay_ms(50);

	uint8_t sw1 = (PIND >> PIND2) & 1;
	uint8_t sw2 = (PIND >> PIND4) & 1;

	if (sw1 != sw1_state) {
		if (sw1 == 0)
			value++;
		sw1_state = sw1;
	}
	if (sw2 != sw2_state) {
		if (sw2 == 0)
			value--;
		sw2_state = sw2;
	}
	display_value();
	/* PCIFR = Pin Change Interrupt Flag Register, it handles the Interrupt flags, putting a logical 1 into one flag cancel its interrupt queue */
	PCIFR |= _BV(PCIF2); //cancel vector queue to handle rebounce
}
