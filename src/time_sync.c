#include "time_sync.h"
#include "suli.h"

/*
 * LOCAL VARIABLES AND FUNCTIONS
 */

static uint32 u32ZCLMutexCount = 0;
void vUnlockZCLMutex(void);
void vLockZCLMutex(void);
void change_time(uint32 time, int position);

static int high_part_read      = 0;
static int low_part_read       = 0;
static uint64 start_time       = 0;
static uint64 local_start_time = 0;

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
	uint64 res = suli_millis() - local_start_time + start_time;
    vUnlockZCLMutex();
    return res;
}

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