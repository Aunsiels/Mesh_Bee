#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include "common.h"

void init_time_sync(void);
int timeHasBeenSynchronised(void);
uint64 getTime(void);
void setHighTime(uint32 time);
void setLowTime(uint32 time);
uint64 getTimeModified(uint64 read_time);

#endif