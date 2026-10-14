#include "twi.h"
#include "uart.h"

#include <util/delay.h>
#include <stdlib.h>

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

typedef struct s_sensor_samples
{
	uint32_t	humidity;
	uint32_t	temperature;
}	t_sensor_samples;

t_sensor_samples samples[3] = {0};

static uint8_t samples_index = 0;

static _Bool	setup();
static _Bool	init_AHT20();
static _Bool	get_aht20_sample();
static _Bool	wait_measurement();
static void		print_average();


int main(void)
{
	_delay_ms(40); // wait for AHT20 to start (5.4.1 p8)
	if (setup())
		INVALID_ADDRESS();
	get_aht20_sample();
	get_aht20_sample(); // get first 2 sample to print average
	while (1)
	{
		get_aht20_sample();
		_delay_ms(2000); // recommended to mesure every 2 sec to avoid rising the sensor temperature (2.4 p5)
		print_average();
	}
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

_Bool	get_aht20_sample()
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
	
	i2c_get_read(); // skip status bit
	uint8_t bytes[5];
	for (int i = 0; i < 5; i++)
	{
		if (i == 4) // dont need last byte
			bytes[i] = i2c_get_read_last();
		else
			bytes[i] = i2c_get_read();
	}
	// third byte send is a combinaison of humidity and temperature bytes as they are coded in 24 bits (3 bytes)
	samples[samples_index].humidity = (((uint32_t)bytes[0] << 12) | ((uint32_t)bytes[1] << 4) | (bytes[2] >> 4));
	samples[samples_index].temperature = ((uint32_t)(bytes[2] & 0x0F) << 16) | ((uint32_t)bytes[3] << 8) | bytes[4]; // 0x0F = 00001111, its a mask to skip the 4 first bytes
	samples_index++;
	if (samples_index == 4)
		samples_index = 0;
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

void	print_average()
{
	uint32_t sum_humidity = 0;
	uint32_t sum_temperature = 0;

	for (uint8_t i = 0; i < 3; i++)
	{
		sum_humidity += samples[i].humidity;
		sum_temperature += samples[i].temperature;
	}

	uint32_t average_humidity = sum_humidity / 3;
	uint32_t average_temperature = sum_temperature / 3;

	double humitidy_percentage = average_humidity / 1048576. * 100; 
	double temperature_celcius = average_temperature / 1048576. * 200 - 50;
	char buf[10] = {0};
	dtostrf(temperature_celcius, 1, 1, buf);
	uart_printstr(buf);
	uart_printstr("°C, ");
	dtostrf(humitidy_percentage, 1, 2, buf);
	uart_printstr(buf);
	uart_printstr("%\r\n");
}