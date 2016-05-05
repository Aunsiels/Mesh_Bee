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
#include "zcl.h"

void DO12_callback(uint32 u32Device, uint32 u32ItemBitmap){
	//We check if the callback was called by D12
	suli_uart_printf(NULL, NULL, "In callback\r\n");
	
	static last_D12 = 0;
	
	uint32 status = u32AHI_DioInterruptStatus();
	if (status & (1 << D12)){
		//To avoid too many interrupts
		if (suli_millis() - last_D12 > 100) {
			last_D12 = suli_millis();
			suli_uart_printf(NULL, NULL, "Interrupt !\r\n");
		}
	}
}

ANALOG_T temp_pin;
ANALOG_T lumi_pin;
const int LUMI = 0;
IO_T button_pin;

void arduino_setup(void)
{
#ifdef TARGET_ROU
    setNodeState(E_MODE_MCU);
	//init_humidity();
	//suli_analog_init(&lumi_pin, LUMI);
	suli_pin_init(&button_pin, D12);
	suli_pin_dir(&button_pin, HAL_PIN_INPUT);
	//Pull up by default
	vAHI_SysCtrlRegisterCallback(DO12_callback);
	//Enable interrupt
	uint32 isr_mask = (1 << D12);
	//The Second argument is to disable
	vAHI_DioInterruptEnable(isr_mask, 0);
	uint32 falling = (1 << D12);
	vAHI_DioInterruptEnable(0, falling); // First argument for rising edge
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

	//hum = read_temperature();
    //hum = random();
    //hum = suli_analog_read(&lumi_pin);
	hum = suli_analog_read(&temp_pin);
	
    uint32 high = (uint32)(ZPS_u64AplZdoGetIeeeAddr() >> 32);
	uint32 low  = (uint32)(ZPS_u64AplZdoGetIeeeAddr());
	
    /*sprintf(tmp, "TEMP%08x%08x%d\r\n",
                high,
                low,
                hum);*/
	
    /*sprintf(tmp, "RAND%08x%08x%d\r\n",
                high,
                low,
                hum);*/
				
    /*sprintf(tmp, "LUMI%08x%08x%d\r\n",
                high,
                low,
                hum);*/			
    
	sprintf(tmp, "TMPIXXXX%d",
                hum);	


	if (bZCL_GetTimeHasBeenSynchronised()){
        uint32 time = u32ZCL_GetUTCTime();
		char * time_ptr = (char *) &time;
		
		uart_printf("Time : %d s\r\n", time);
		
		tmp[4] = time_ptr[0];
		tmp[5] = time_ptr[1];
		tmp[6] = time_ptr[2];
		tmp[7] = time_ptr[3];
		
		PCK_vApiSpecDataFrame(&apiSpec, 0xec, 0x00, tmp, strlen(tmp+8) + 8);

        /* Air to Coordinator */
        uint16 size = i32CopyApiSpec(&apiSpec, tmp);
	
        if(API_bSendToAirPort(UNICAST, 0x0000, tmp, size))
        {
            suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", random());
        }
	} else {
		uart_printf("Nothing to read\r\n");
	}
	
    /*hum = read_humidity();
    sprintf(tmp, "HUMI%08x%08x%d\r\n",
                high,
                low,
                hum);
				
	PCK_vApiSpecDataFrame(&apiSpec, 0xec, 0x00, tmp, strlen(tmp));

    size = i32CopyApiSpec(&apiSpec, tmp);
    if(API_bSendToAirPort(UNICAST, 0x0000, tmp, size))
    {
        suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", random());
    }*/
	
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
