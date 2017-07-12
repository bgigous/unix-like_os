#ifndef _SAS_H
#define _SAS_H

#include "types.h"
#include "lib.h"
#include "term_driver.h"

#define MIN_YEAR 	1956
#define SIZE		22
#define LEAP		4
#define MONTHS		12
#define DIV_BASE	10
#define IDX_OFF		3
void calendar_handler(int32_t month, int32_t year);

#endif
