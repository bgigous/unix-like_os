#ifndef _RNS_H
#define _RNS_H

#include "types.h"
#include "lib.h"
#define RTC_OUT 0x70
#define RTC_IN 0x71

//functions for initializing rtc and handling intterupt
void handle_rtc(void);
void rtc_init(void);
//have it as an int32_t - enforces same size of return value across all machines
int32_t rtc_open(const uint8_t *filename);
int32_t rtc_close(int32_t fd);
int32_t set_freq(unsigned long arg);
int32_t rtc_write(int32_t fd,const void *buf, int32_t nbytes);
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);
#endif

