#include "time_sync.h"
#include "suli.h"
#include "common.h"

#define N_SAMPLES 10

static PDM_tsRecordDescriptor   g_correction;

/*
 * LOCAL VARIABLES AND FUNCTIONS
 */

static uint32 u32ZCLMutexCount = 0;
void vUnlockZCLMutex(void);
void vLockZCLMutex(void);
void change_time(uint32 time, int position);

static volatile int    high_part_read      = 0;
static volatile int    low_part_read       = 0;
static volatile uint64 start_time       = 0;
static volatile uint64 local_start_time = 0;
static volatile float correction       = 0.f;
static IO_T            pin;

/*
 * LOCAL DEFINITIONS
 */

/****************************************************************************
 *
 * NAME: vUnlockZCLMutex
 *
 * DESCRIPTION:
 * Releases and maintains a counting mutex
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vUnlockZCLMutex(void)
{
	u32ZCLMutexCount--;
	if (u32ZCLMutexCount == 0)
	{
		OS_eExitCriticalSection(ZCL);
	}
}

/****************************************************************************
 *
 * NAME: vLockZCLMutex
 *
 * DESCRIPTION:
 * Grabs and maintains a counting mutex
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vLockZCLMutex(void)
{
	// We have a counter here to be sure that a task will not enter the
	// critical section twice
	if (u32ZCLMutexCount == 0)
	{
		OS_eEnterCriticalSection(ZCL);
	}
	u32ZCLMutexCount++;
}

/****************************************************************************
 *
 * NAME: change_time
 *
 * DESCRIPTION:
 * Change a part of the current time
 *
 * PARAMETERS:
 *  time : The current time to set
 *  position : 0 for low part, 1 for high
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

void change_time(uint32 time, int position)
{
	// Uncorrect parameter
	if (position != 0 && position != 1) return;
	vLockZCLMutex();

	uint32 * start = (uint32 *) &start_time;
	uint32 * local = (uint32 *) &local_start_time;
	uint64 current_time = suli_millis();
	uint32 * ptr_current = (uint32 *) &current_time;
	
	start[position] = time;
    local[position] = ptr_current[position];

    if (position == 0) {
    	low_part_read = 1;
    } else {
        high_part_read = 1;
    }

    vUnlockZCLMutex();
}

/*
 * EXPORTED FUNCTIONS
 */

/****************************************************************************
 *
 * NAME: init_time_sync
 *
 * DESCRIPTION:
 * Initialize the local variables for time synchronization
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void init_time_sync(void)
{
	// Reset all variables
	vLockZCLMutex();
	high_part_read   = 0;
    low_part_read    = 0;
    start_time       = 0;
    local_start_time = 0;
    vUnlockZCLMutex();
    // Load coefficient parameter
    PDM_eLoadRecord(&g_correction, REC_ID2, &correction, sizeof(correction), FALSE);
    suli_pin_init(&pin, D1);
    suli_pin_dir(&pin, HAL_PIN_INPUT);
    suli_pin_init(&pin, D18);
    suli_pin_dir(&pin, HAL_PIN_INPUT);
    uint32 mask = (1 << D1);
    vAHI_DioSetDirection(mask, 0);
    vAHI_DioInterruptEdge(0, mask); // First argument for rising edge
    vAHI_DioInterruptEnable(mask, 0);
}

/****************************************************************************
 *
 * NAME: timeHasBeenSynchronised
 *
 * DESCRIPTION:
 * Tells if the time have been correctly synchronized
 *
 *
 * RETURNS:
 * 0 if not synchronized, 1 otherwise
 *
 * TODO : Naming covention for function ?
 *
 ****************************************************************************/
int timeHasBeenSynchronised(void)
{
	vLockZCLMutex();
	int res = high_part_read & low_part_read;
    vUnlockZCLMutex();
    return res;
}

/****************************************************************************
 *
 * NAME: getTime
 *
 * DESCRIPTION:
 * Return the current synchronized time. If the time have not been
 * synchronized yet, the value means nothing. Check timeHasBeenSynchronized
 * first.
 *
 *
 * RETURNS:
 * The current synchronized time
 *
 ****************************************************************************/
uint64 getTime(void)
{
	vLockZCLMutex();
    int d_temp = suli_millis() - local_start_time;
    int delta = d_temp / correction;
	uint64 res = delta + start_time;
    vUnlockZCLMutex();
    return res;
}

/*uint64 getTimeModified(uint64 read_time)
{
	static uint64 xi[N_SAMPLES];
    static uint64 yi[N_SAMPLES];
    static int count            = 0;
    static double a             = 0;
    static double b             = 0;

    int i = 0;

    uint64 res;

	vLockZCLMutex();
    uint64 x = suli_millis() - local_start_time;
    if (count == N_SAMPLES) {
        //Compute a and b
        double x_mean = 0.;
        double y_mean = 0.;
        for(i = 0; i < N_SAMPLES; i++){
            x_mean += xi[i];
            y_mean += yi[i];
        }
        x_mean /= N_SAMPLES;
        y_mean /= N_SAMPLES;

        double top    = 0.;
        double bottom = 0.;

        for (i = 0; i < N_SAMPLES; i++){
            double temp = (xi[i] - x_mean);
            top += temp * (yi[i] - y_mean);
            bottom += temp * temp;
        }

        a = top / bottom;
        b = y_mean - a * x_mean;

        count = 0;
    } else if (count < N_SAMPLES) {
        uint64 y = read_time - start_time;
        xi[count] = x;
        yi[count] = y;
        count++;
    }

    if (a == 0) return read_time;

    suli_uart_printf(NULL, NULL, "a : ");
    suli_uart_write_float(NULL, NULL, a, 6);
    suli_uart_printf(NULL, NULL, "\r\n");

    suli_uart_printf(NULL, NULL, "b : ");
    suli_uart_write_float(NULL, NULL, b, 6);
    suli_uart_printf(NULL, NULL, "\r\n");

    double modif = x * a + b;
    suli_uart_write_float(NULL, NULL, modif, 6);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_write_float(NULL, NULL, x, 6);
    suli_uart_printf(NULL, NULL, "\r\n");
    res = modif + start_time;
    suli_uart_printf(NULL, NULL, "res : %ld\r\n", ((int *) &res)[1]);
    suli_uart_printf(NULL, NULL, "res true : %ld\r\n", ((int *) &read_time)[1]);
    
    vUnlockZCLMutex();
    return res;
}*/

/****************************************************************************
 *
 * NAME: setHighTime
 *
 * DESCRIPTION:
 * Set the high part of the time stamp
 *
 * PARAMETERS:
 *  time : The current time to set
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void setHighTime(uint32 time)
{
    change_time(time, 1);
}

/****************************************************************************
 *
 * NAME: setLowTime
 *
 * DESCRIPTION:
 * Set the high part of the time stamp
 *
 * PARAMETERS:
 *  time : The current time to set
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void setLowTime(uint32 time)
{
    change_time(time, 0);
}

/****************************************************************************
 *
 * NAME: setTimeCorrection
 *
 * DESCRIPTION:
 * Set the correction parameter
 *
 * PARAMETERS:
 *  corr : the correction
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
 void setTimeCorrection(float corr)
 {
    correction = corr;
    PDM_vSaveRecord(&g_correction);
 }


/****************************************************************************
 *
 * NAME: getCorrection
 *
 * DESCRIPTION:
 * Get the correction parameter
 *
 *
 * RETURNS:
 * the correction
 *
 ****************************************************************************/
 float getTimeCorrection()
 {
    return correction;
 }