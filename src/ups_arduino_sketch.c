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
#include "utils_meshbee.h"
#include "LSM9DS0.h"
#include "time_sync.h"

IO_T led_pin;

ANALOG_T temp_pin;
ANALOG_T lumi_pin;
const int LUMI = 0;

IO_T button_pin = D0;

static LSM_properties prop;

void arduino_setup(void)
{
#ifdef TARGET_ROU
    setNodeState(E_MODE_MCU);

    // Set period loop
    char * aj = "ATMF3000";
    char * aj2 = "ATMF";
    API_i32AtCmdProc(aj, 8);
    // Prints the value of the loop
    API_i32AtCmdProc(aj2, 4);
	
	suli_pin_init(&led_pin, D18);
	suli_pin_dir(&led_pin, HAL_PIN_OUTPUT);
	
	//init_humidity();
	//suli_analog_init(&lumi_pin, LUMI);
	//Pull up by default
	//Enable interrupt
	uint32 mask = (1 << D0);
	//The Second argument is to disable
	vAHI_DioSetDirection(mask, 0);
	vAHI_DioInterruptEdge(0, mask); // First argument for rising edge
	vAHI_DioInterruptEnable(mask, 0);
	/*LSM_parameters params;
    params.gScl = G_SCALE_245DPS;
    params.aScl = A_SCALE_2G;
    params.mScl = M_SCALE_2GS;
    params.gODR = G_ODR_95_BW_125;
    params.aODR = A_ODR_50;
    params.mODR = M_ODR_50;
    uint16 status = init_LSM(params);
    suli_uart_printf(NULL, NULL, "Status : %d\r\n", status);

    configTapInt(1.5, 16);

    calLSM9DS0(&prop);
    /*suli_uart_printf(NULL, NULL, "gbias : ");
    suli_uart_write_float(NULL, NULL, prop.gbias[0], 10);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_write_float(NULL, NULL, prop.gbias[1], 10);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_write_float(NULL, NULL, prop.gbias[2], 10);
    suli_uart_printf(NULL, NULL, "\r\nabias : ");
    suli_uart_write_float(NULL, NULL, prop.abias[0], 10);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_write_float(NULL, NULL, prop.abias[1], 10);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_write_float(NULL, NULL, prop.abias[2], 10);
    suli_uart_printf(NULL, NULL, "\r\n");*/

    suli_uart_printf(NULL, NULL, "Correction parameter : ");
    suli_uart_write_float(NULL, NULL, getTimeCorrection(), 10);
    suli_uart_printf(NULL, NULL, "\r\n");

#endif

    suli_analog_init(&temp_pin, TEMP);
}

void arduino_loop(void)
{
#ifdef TARGET_COO
    vDelayMsec(100);
    suli_uart_printf(NULL, NULL, "random:%d\r\n", random());
#elif TARGET_ROU

    //unsigned int hum = 0;

	//hum = read_temperature();
    //hum = random();
    //hum = suli_analog_read(&lumi_pin);
	//hum = suli_analog_read(&temp_pin);	
    
    //send_frame("TMPI", hum);
    //send_frame("RAND", hum);
	//send_frame("LUMI", hum);
    //send_frame("TEMP", hum);
	
    /*hum = read_humidity();
	send_frame("HUMI", hum);
	*/

    //readAccel(&prop);
    /*readMag(&prop);
    readGyro(&prop);*/
    /*suli_uart_printf(NULL, NULL, "A : ");
    suli_uart_write_float(NULL, NULL, calcAccel(prop.ax), 3);
    suli_uart_printf(NULL, NULL, " , ");
    suli_uart_write_float(NULL, NULL, calcAccel(prop.ay), 3);
    suli_uart_printf(NULL, NULL, " , ");
    suli_uart_write_float(NULL, NULL, calcAccel(prop.az), 3);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_printf(NULL, NULL, "A raw : %d %d %d\r\n", prop.ax, prop.ay, prop.az);*/
    /*suli_uart_printf(NULL, NULL, "G : ");
    suli_uart_write_float(NULL, NULL, calcGyro(prop.gx), 3);
    suli_uart_printf(NULL, NULL, " , ");
    suli_uart_write_float(NULL, NULL, calcGyro(prop.gy), 3);
    suli_uart_printf(NULL, NULL, " , ");
    suli_uart_write_float(NULL, NULL, calcGyro(prop.gz), 3);
    suli_uart_printf(NULL, NULL, "\r\n");
    suli_uart_printf(NULL, NULL, "M : ");
    suli_uart_write_float(NULL, NULL, calcMag(prop.mx), 3);
    suli_uart_printf(NULL, NULL, " , ");
    suli_uart_write_float(NULL, NULL, calcMag(prop.my), 3);
    suli_uart_printf(NULL, NULL, " , ");
    suli_uart_write_float(NULL, NULL, calcMag(prop.mz), 3);
    suli_uart_printf(NULL, NULL, "\r\n");*/

    //suli_delay_ms(3000);

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
