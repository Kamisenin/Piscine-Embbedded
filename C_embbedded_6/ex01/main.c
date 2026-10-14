#include "twi.h"
#include "uart.h"

#include <util/delay.h>

#define AHT20_ADDR 0x38 // address of every AHT20, section 5.3 p8 (https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf)

#define AHT_INIT 		0xBE
#define AHT_INIT_PARAM	0x08

#define AHT_START_MEASURE 0xAC
#define AHT_MEASURE_PARAM 0x33

#define INVALID_TRANSMISSION() 							\
{														\
	uart_printstr("Invalid Transmission : NACK\r\n");	\
	i2c_stop();											\
	return (1); 										\
}

#define INVALID_ADDRESS()																\
{																						\
	uart_printstr("Could'not find AHT address, are you sure its up/connected ?\r\n");	\
	return (1);																			\
}


static _Bool	setup();
static _Bool	init_AHT20();
static _Bool	get_aht20_measurement();
static _Bool	wait_measurement();


int main(void)
{
	_delay_ms(40); // wait for AHT20 to start (5.4.1 p8)
	if (setup())
		INVALID_ADDRESS();
	while (1)
		get_aht20_measurement();
}

_Bool	setup()
{
	uart_init();
	i2c_init();
	if (init_AHT20())
		return (1);
	return (0);
}

_Bool	init_AHT20()
{
	if (i2c_init_read(AHT20_ADDR))
		INVALID_ADDRESS();

	if (i2c_get_read_last() >> 3 & 1) // check whether the  AHT is already calibrated, if it is, skip calibration
		return (0);

	if (i2c_init_write(AHT20_ADDR))
		INVALID_ADDRESS();
	/*
	 * Initialize AHT20 (need to send INIT command and empty byte)
	 */
	i2c_write(AHT_INIT);		if (TW_STATUS != TW_MT_DATA_ACK) INVALID_TRANSMISSION();
	i2c_write(AHT_INIT_PARAM);	if (TW_STATUS != TW_MT_DATA_ACK) INVALID_TRANSMISSION();
	i2c_write(0);				if (TW_STATUS != TW_MT_DATA_ACK) INVALID_TRANSMISSION();
	i2c_stop();
	_delay_ms(10); // wait initialisation time (5.4.1 p8)
	return (0);
}

_Bool	get_aht20_measurement()
{
	if (i2c_init_write(AHT20_ADDR))
        INVALID_ADDRESS();
	i2c_write(AHT_START_MEASURE);	if (TW_STATUS != TW_MT_DATA_ACK) INVALID_TRANSMISSION();
	i2c_write(AHT_MEASURE_PARAM); 	if (TW_STATUS != TW_MT_DATA_ACK) INVALID_TRANSMISSION();
	// send empty byte to complete measurement command (5.4.2 p8)
	i2c_write(0);  					if (TW_STATUS != TW_MT_DATA_ACK) INVALID_TRANSMISSION();
	
	if (wait_measurement())
		INVALID_ADDRESS();
	
	if (i2c_init_read(AHT20_ADDR))
		INVALID_ADDRESS();
	
	for (int i = 0; i < 7; i++)
	{
		if (i == 6)
			i2c_read_last();
		else
			i2c_read();
		uart_tx(' ');
			
	}
	uart_printstr("\r\n");
	return (0);
}

_Bool	wait_measurement()
{
	_delay_ms(80); // wait for measurement (5.4.2 p8)
	uint8_t measurement_status = 0;
	do {
		_delay_ms(10);
		if (i2c_init_read(AHT20_ADDR))
		{
			i2c_stop();
			return (1);
		}
		measurement_status = i2c_get_read_last();
		
	} while ((measurement_status >> 7) & 1);
	return (0);
}