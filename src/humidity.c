#include "suli.h"
#include "humidity.h"

#define HTDU21D_ADDRESS (uint8) 0x40  //Unshifted 7-bit I2C address for the sensor

#define TRIGGER_TEMP_MEASURE_HOLD  (uint8) 0xE3
#define TRIGGER_HUMD_MEASURE_HOLD  (uint8) 0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  (uint8) 0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  (uint8) 0xF5
#define WRITE_USER_REG  (uint8) 0xE6
#define READ_USER_REG  (uint8) 0xE7
#define SOFT_RESET  (uint8) xFE

/*
  Initializes the humidity sensor for an i2c communication
*/
void init_humidity(void) {
	suli_i2c_init(NULL);
}

/*
    Gives the humidity
*/
unsigned int read_humidity(void) {
	uint8 data = TRIGGER_HUMD_MEASURE_NOHOLD;
	suli_i2c_write(NULL, HTDU21D_ADDRESS, &data, 1);

	uint8 buff[3] = {0};
	
	suli_delay_ms(100);
	
	uint8 msb, lsb, checksum;
	
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &msb, 1);
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &lsb, 1);
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &checksum, 1);
	
	unsigned int rawHumidity = ((unsigned int) msb << 8) | (unsigned int) lsb;

	rawHumidity &= 0xFFFC; //Zero out the status bits but keep them in place
	
	//Given the raw humidity data, calculate the actual relative humidity
	//float tempRH = rawHumidity / (float)65536; //2^16 = 65536
	//float rh = -6 + (125 * tempRH); //From page 14
	
	return rawHumidity;
}

/*
    Gives the temperature
*/
unsigned int read_temperature(void) {
	uint8 data = TRIGGER_TEMP_MEASURE_NOHOLD;
	suli_i2c_write(NULL, HTDU21D_ADDRESS, &data, 1);
	
	suli_delay_ms(100);
	
	uint8 msb, lsb, checksum;
	
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &msb, 1);
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &lsb, 1);
	suli_i2c_read(NULL, HTDU21D_ADDRESS, &checksum, 1);
	
	suli_uart_printf(NULL, NULL, "MSB : %d\r\n", msb);
	suli_uart_printf(NULL, NULL, "LSB : %d\r\n", lsb);
	suli_uart_printf(NULL, NULL, "checksum : %d\r\n", checksum);

	unsigned int rawTemperature = ((unsigned int) msb << 8) | (unsigned int) lsb;

	rawTemperature &= 0xFFFC; //Zero out the status bits but keep them in place

	suli_uart_printf(NULL, NULL, "READ : %d\r\n", rawTemperature);
	
	//Given the raw temperature data, calculate the actual temperature
	//float tempTemperature = rawTemperature / (float)65536; //2^16 = 65536
	//float realTemperature = (float)(-46.85 + (175.72 * tempTemperature)); //From page 14

	return rawTemperature;  
}
