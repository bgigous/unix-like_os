#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "lib.h"
#include "types.h"
#include "paging.h"

// all system calls return an integer (in the form of eax)
int32_t system_call(void); // doesn't take any arguments, they are store in registers

#endif
