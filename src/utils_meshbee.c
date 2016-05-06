#include "utils_meshbee.h"
#include "common.h"
#include "suli.h"
#include "firmware_api_pack.h"
#include "firmware_at_api.h"
#include "firmware_aups.h"
#include "zcl.h"

void send_frame(char* name, int data){
	uint8 tmp[sizeof(tsApiSpec)]={0};
    tsApiSpec apiSpec;
	//Construct the frame
	sprintf(tmp, "NAMEXXXX%d",
            data);
	
    // Include the name
    tmp[0] = name[0];
    tmp[1] = name[1];
    tmp[2] = name[2];
    tmp[3] = name[3];
	
	// Include the time stamp
	if (bZCL_GetTimeHasBeenSynchronised()){
        uint32 time = u32ZCL_GetUTCTime();
		char * time_ptr = (char *) &time;
		
		/* To debug, TODO remove it */
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
			/* To debug, TODO remove it */
            suli_uart_printf(NULL, NULL, "<HeartBeat%d>\r\n", random());
        }
	} else {
		/* To debug, TODO remove it */
		uart_printf("No time data.\r\n");
	}
}