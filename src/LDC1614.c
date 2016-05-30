#include "LDC1614.h"

uint16 readBytes(uint8 subAddr);
void writeBytes(uint8 subAddr, uint16 data);

uint16 readBytes(uint8 subAddr)
{
	uint16 res;

	suli_i2c_write(NULL, ADDR_LOW, &subAddr, 1);
    suli_i2c_read(NULL, ADDR_LOW, &res, 2);
    return res;
}

void writeBytes(uint8 subAddr, uint16 data)
{
	uint8 tosend[3];
	uint8 * data_ptr = (uint8 *) &data;
	tosend[0] = subAddr;
	// Most significative bit first
	tosend[1] = data_ptr[0];
	tosend[2] = data_ptr[1];
    suli_i2c_write(NULL, ADDR_LOW, tosend, 3);
}

void init_LDC1614(void)
{
	// Init I2C
	suli_i2c_init(NULL);
	// Reference count for 1kSPS
	writeBytes(RCOUNT_CH0, 0x04D6);
	writeBytes(RCOUNT_CH1, 0x04D6);
	// Minimum settling time for the choosen sensor
	writeBytes(SETTLECOUNT_CH0, 0x000A);
	writeBytes(SETTLECOUNT_CH1, 0x000A);
	// CH0_FIN_DIVIDER = 1, CH0_FREF_DIVIDER = 2
	writeBytes(CLOCK_DIVIDERS_CH0, 0x1002);
	writeBytes(CLOCK_DIVIDERS_CH1, 0x1002);
	// Can be changed from default to report status and error conditions
	writeBytes(ERROR_CONFIG, 0x0000);
	// Enable Ch 0 and Ch 1 (sequential mode), set Input deglitch bandwidth to 3.3MHz
	writeBytes(MUX_CONFIG, 0x820C);
	// Sets sensor drive current on ch 0
	writeBytes(DRIVE_CURRENT_CH0, 0x9000);
	writeBytes(DRIVE_CURRENT_CH1, 0x9000);
	// disable auto-amplitude correction and auto-calibration, enable full current
    // drive during sensor activation, select external clock source, wake up device
    // to start conversion. This register write must occur last because device
    // configuration is not permitted while the LDC is in active mode
	writeBytes(CONFIG, 0x1601);
}

uint32 read_sensor0()
{
	uint32 res = 0;
	uint16 * res_ptr = (uint16 *) &res;
	// Remove error bits
    res_ptr[0] = readBytes(DATA_MSB_CH0) & 0x0FFF;
    // Must be read after MSB for data coherency
    res_ptr[1] = readBytes(DATA_LSB_CH0);
    return res;
}

uint32 read_sensor1()
{
	uint32 res = 0;
	uint16 * res_ptr = (uint16 *) &res;
    res_ptr[0] = readBytes(DATA_MSB_CH1) & 0x0FFF;
    // Must be read after MSB for data coherency
    res_ptr[1] = readBytes(DATA_LSB_CH1);
    return res;
}