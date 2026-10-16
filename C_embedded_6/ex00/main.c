#include "twi.h"
#include "uart.h"

int main(void)
{
	i2c_init();
	uart_init();
	i2c_start();
	wait_transmission();
	uart_putnbr_base(TW_STATUS, 16);
	uart_printstr("\r\n");
	if (TW_STATUS == TW_START) // Bus has been successfull taken
	{
		// TWDR = TW Data Register. Set this register to send data.
		// In first transmission send the address of the slave you want to communicate with
		TWDR = (AHT20_addr << 1) | TW_WRITE; // Move AHT adress 1 byte to place read/write modde byte
		TWCR = TW_ACK;
		wait_transmission();
		uart_putnbr_base(TW_STATUS, 16);
		uart_printstr("\r\n");
		if (TW_STATUS == TW_MT_SLA_ACK)
		{
			TWDR = 0;
			TWCR = TW_ACK;
			wait_transmission();
			uart_putnbr_base(TW_STATUS, 16);
			uart_printstr("\r\n");
		}
		i2c_stop();
		while (TWCR & _BV(TWSTO));
	}
}	