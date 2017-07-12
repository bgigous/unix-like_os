#ifndef _TERMINALS_H
#define _TERMINALS_H

#include "task.h"
#include "types.h"
#include "x86_desc.h"
#include "pcb.h"
#include "lib.h"
#include "paging.h"
#include "sys_handler.h"
#include "keyboard.h"
#include "i8259.h"

void terminal_switch(int pid);
void save_vid_memory(int pid);
void reload_vid_memory(int pid);

#endif
