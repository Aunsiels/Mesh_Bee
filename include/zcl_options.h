#ifndef ZCL_OPTIONS_H
#define ZCL_OPTIONS_H

#include <jendefs.h>

#define CLD_TIME
#ifdef TARGET_COO
#define TIME_SERVER
#else
#define TIME_CLIENT
#endif

#endif