#ifndef UTILS_MESHBEE_H
#define UTILS_MESHBEE_H

/*
 * send_frame Sends a custom frame to the coordinator according to the following protocol :
 *  - 4 bytes to identify the value measured
 *  - 4 bytes for the time stamp
 *  - the data measured.
 *
 * name : the id of the value measured, on 4 bytes
 * data : the data measured, in an int
 */
 
 void send_frame(char* name, int data);


#endif
