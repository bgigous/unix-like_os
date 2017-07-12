#include "terminals.h"

#define INTR_STACK_SIZE 20

void terminal_switch(int pid)
{
	if ( pid > 2 )
	{
		printf( "uh oh, term_switch pid > 2\n");
	}

	// in case we are switching to the very same terminal
	if(pid == CURR_TASK->pid)
		return;
	
	//save video memory
	save_vid_memory(CURR_TASK->pid);

	//reset_cursor(0);
	pcb_t * switch_to = get_pcb_from_pid(pid);

	// get the "topmost" program
	while (switch_to->child != NULL)
	{
		switch_to = switch_to->child;
		pid = switch_to->pid;
	}
	cr3_switch( &pdir[pid] );

	//load old vid memory
	reload_vid_memory(pid);

	//switch to the correct process
	CURR_TASK = switch_to;
}

void save_vid_memory(int pid)
{
	//save current video memory into corresponding page from 10-12MB
	uint8_t *src = (uint8_t *)VIDEO_MEM;
	// below might be screwy with the &
	//uint8_t *dest = 0xA00000;
	uint32_t size = PAGE_SIZE_4KB;
	memcpy((uint8_t *)(0xA00000+(pid*0x50000)), src, size);	
	//memcpy((uint8_t *)(0xA00000), src, size);	
}

void reload_vid_memory(int pid)
{
	//load the video buffer memory from the corresponding page in 10-12MB back
	uint8_t *dest = (uint8_t *)VIDEO_MEM;
	memcpy(dest, (uint8_t *)(0xA00000+(pid*0x50000)), PAGE_SIZE_4KB);	
	//memcpy( dest,(uint8_t *)(0xA00000), PAGE_SIZE_4KB);	
}
