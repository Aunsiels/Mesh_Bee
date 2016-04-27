/*
 * ups_arduino_sketch.c
 * User programming space, arduino sketch file
 * Firmware for SeeedStudio Mesh Bee(Zigbee) module
 *
 * Copyright (c) NXP B.V. 2012.
 * Spread by SeeedStudio
 * Author     : Oliver Wang
 * Create Time: 2014/3
 * Change Log :
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "common.h"
#include "firmware_api_pack.h"
#include "firmware_at_api.h"
#include "firmware_aups.h"
#include "firmware_sleep.h"
#include "suli.h"
#include "humidity.h"

ANALOG_T temp_pin;

void arduino_setup(void)
{
#ifdef TARGET_ROU
    setNodeState(E_MODE_MCU);
	init_humidity();
#endif

    suli_analog_init(&temp_pin, TEMP);
}

void arduino_loop(void)
{
#ifdef TARGET_COO
    vDelayMsec(100);
    suli_uart_printf(NULL, NULL, "random:%d\r\n", random());
#elif TARGET_ROU
	unsigned int hum = 0;
	uint8 tmp[sizeof(tsApiSpec)]={0};
    tsApiSpec apiSpec;

	hum = read_temperature();
    /*hum = random();*/

    uint32 high = (uint32)(ZPS_u64AplZdoGetIeeeAddr() >> 32);
	uint32 low  = (uint32)(ZPS_u64AplZdoGetIeeeAddr());

    sprintf(tmp, "TEMP%08x%08x%d\r\n",
                high,
                low,
                hum);

    /*sprintf(tmp, "RAND%08x%08x%d\r\n",
                high,
                low,
                hum);*/

    PCK_vApiSpecDataFrame(&apiSpec, 0xec, 0x00, tmp, strlen(tmp));

    /* Air to Coordinator */
    uint16 size = i32CopyApiSpec(&apiSpec, tmp);
    if(API_bSendToAirPort(UNICAST, 0x0000, tmp, size))
    {
        suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", random());
    }
	vDelayMsec(3000);
#else
    /* Finish user job */
    static jobCnt = 0;
    uint8 tmp[sizeof(tsApiSpec)]={0};
    tsApiSpec apiSpec;

    int16 temper = suli_analog_read(temp_pin);
    sprintf(tmp, "E-HeartBeat:%ld\r\n", temper);
    PCK_vApiSpecDataFrame(&apiSpec, 0xec, 0x00, tmp, strlen(tmp));

    /* Air to Coordinator */
    uint16 size = i32CopyApiSpec(&apiSpec, tmp);
    if(API_bSendToAirPort(UNICAST, 0x0000, tmp, size))
    {
        suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", jobCnt);
        jobCnt++;
    }
    if(10 == jobCnt)
    {
        jobCnt = 0;
        Sleep(3000);
    }

#endif
}
