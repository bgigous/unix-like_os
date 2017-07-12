#include "sys_handler.h"

/* the file operations table for rtc, directory, and actual files, respectively */
static f_ops rtc_ops =	{ rtc_open, 	rtc_read, 	rtc_write, 	rtc_close };
static f_ops dir_ops =	{ dir_open, 	dir_read,	dir_write, 	dir_close };
static f_ops file_ops=	{ file_open,	file_read,	file_write,	file_close };

/* Signal default actions declarations */
static void div_zero_default( void );
static void seg_fault_default( void );
static void interrupt_default( void );
static void alarm_default( void );
static void user_default( void );

/* Signal default actions definitions */

void div_zero_default( void )
{
	// kill the task
	halt( 0 );
}

void seg_fault_default( void )
{
	// kill the task
	halt( 0 );
}

void interrupt_default( void )
{
	// kill the task
	halt( 0 );
}

void alarm_default( void )
{
	// ignore
}

void user_default( void )
{
	// ignore
}

/* the process currently running */
pcb_t *CURR_TASK;

/* represents the active processes */
static int32_t processes[MAX_PROC] = { 0, 0, 0, 0, 0, 0 };

/* stores the arguments for the command */
uint8_t *arg_temp = (uint8_t*)ARG_TEMP_ADDR;
uint8_t *arg2_temp = (uint8_t*)ARG2_TEMP_ADDR;
/*
*	Name: halt
*	Summary: kills a process and switches to its parent task
*   Inputs: status -- some kind of status
*	Outputs: none
*	Return Value: returns the status (indirectly)
*/
int32_t halt(uint8_t status)
{
	unsigned long flags;
	cli_and_save(flags);
	/* convert status from 8-bit to 32-bit return value */
	uint32_t halt_retval = (uint32_t)status;

	pcb_t * parent_task = CURR_TASK->parent;
	if ( parent_task == NULL )
	{
		printf("error: halt: cannot exit main shell\n");
		switch_to_process( CURR_TASK->pid );
	}

	int32_t pid_to_release = CURR_TASK->pid;
	if ( release_pid( pid_to_release ) == -1 )
	{
		printf("error: halt: could not release pid\n");
		return -1;
	}

	if ( close_pcb( CURR_TASK ) == -1 )
	{
		printf("error: halt: could not close pcb\n");
		return -1;
	}

	/* switch to parent  */

	NUM_PROC--;

	CURR_TASK = parent_task;
	parent_task->child = NULL;
	cr3_switch( &pdir[CURR_TASK->pid] );
	tss.esp0 = kernel_stack_for_proc( CURR_TASK->pid );

	restore_flags(flags);

	asm volatile (
		"MOVL	%0, %%eax			;"
		"MOVL	%1, %%esp			;"
		"MOVL	%2, %%ebp			;"
		"JMP	halt_ret			;"
		:
		: "r" (halt_retval), "r" (CURR_TASK->switch_ESP), "r" (CURR_TASK->switch_EBP)
		: "%eax"
	);
	return 0; // dead code lol
}

/*
*	Name: execute
*	Summary: starts and launches a process
*   Inputs: command -- the name of the executable file, along with its arguments
*	Outputs: none
*	Return Value: 	-1 for error
*					256 if program dies by an exception
*					0 to 255 if program executes halt
*/
int32_t execute(const uint8_t* command)
{	
	if ( command == NULL ) return -1;
	
	/* can we run more processes or not? */
	if(NUM_PROC >= MAX_PROC)
	{
		printf("error: execute: cannot run any more tasks\n");
		return 0;
		/* return success so we don't get a silly "abnormal" error */
	}

	/* clear the previous argument (for sanity) */
	int e;
	for(e = 0; e < FILE_ARRAY_SIZE; e++)
	{
		arg_temp[e] = '\0';
		arg2_temp[e] = '\0';
	}
			
	/* parse the arguments */
	int i, j = 0, k = 0;

	uint8_t *exe_name = (uint8_t*)EXE_NAME_ADDR;

	/* get the executable name */
	for (i=0; command[i] != ' ' && command[i]!=0; i++)
	{
		exe_name[i] = command[i];
	}
	exe_name[i] = '\0';

	/* find where the arguments begin */
	while(command[i] == ' ')
		i++;
	j = i;

	/* clear the screen and show a fishy */
	if(strncmp((char *)exe_name, "fish", 4) == 0)
	{
		reset_cursor(0);
	}

	/* clear the screen */
	if(strncmp((char *)exe_name, "clear", 5) == 0)
	{
		reset_cursor(0);
		return 0;
	}

	/* special executables c: */
	if( strncmp((char *)exe_name, "cat", 3) == 0 
		|| strncmp((char*)exe_name, "grep", 4) == 0
		|| strncmp((char*)exe_name, "cc", 2) == 0
		|| strncmp((char*)exe_name, "cal", 3) == 0
	)
	{
		int fg = 0;
		for(fg = 0; fg < FILE_ARRAY_SIZE; fg++)
			arg_temp[fg] = '\0';
		for(fg = 0; fg < FILE_ARRAY_SIZE; fg++)
			arg2_temp[fg] = '\0';
		while(command[j] != ' ' && command[j] != '\0')
		{
			arg_temp[k] = command[j];
			k += 1;
			j += 1;
		}
		arg_temp[k] = '\0';
		j+=1;
		while(command[j] == ' ')
			j++;
		k = 0;
		while(command[j] != ' ' && command[j] != '\0')
		{
			arg2_temp[k] = command[j];
			k += 1;
			j += 1;
		}
		arg2_temp[k] = '\0';
		//terminal color changing handling
		if(strncmp((char*)exe_name, "cc", 2) == 0)
		{
			if(strlen((int8_t *)arg_temp) == 0)
				set_term_color(0);
			else
				set_term_color(*((int8_t *)arg_temp)-CHAR_OFFSET);
			reset_cursor(0);
			return 0;
		}
		//calendar handling
		if(strncmp((char*)exe_name, "cal", 3) == 0)
		{
			int32_t year_val = 0, month_val = 0, ee = 0;
			for(k = strlen((int8_t *)arg_temp); k > 0; k--)
			{
				month_val += (((int32_t)arg_temp[k-1]) - CHAR_OFFSET) * pow(POW_BASE, ee);
				ee++;
			}
			ee = 0;
			for(k = strlen((int8_t *)arg2_temp); k > 0; k--)
			{
				year_val += (((int32_t)arg2_temp[k-1]) - CHAR_OFFSET) * pow(POW_BASE, ee);
				ee++;
			}
			if(strlen((int8_t *)arg2_temp) == 0)
				year_val = 2014;
			calendar_handler(month_val, year_val);
			return 0;
		}
	}

	else
	{
		arg_temp[k] = '\0';
	}

	dentry_t dentry;
	if( -1 == read_dentry_by_name( exe_name, &dentry)) 
	{
		//printf("error: execute: couldn't ready executable directory entry\n");
		return -1;
	}

	/* read beginning of executable, check if it's an executable */
	uint8_t buf[ELF_BYTES];
	read_data( dentry.inode_idx, 0, buf, ELF_BYTES );
	
	/* do we find the magic number? */
	if( !( (buf[0] == 0x7f) && (buf[1] == 'E') && (buf[2] == 'L') && (buf[3] == 'F')) )
	{
		printf("ERROR file is not executable\n");
		/* no :c */
		return -1;
	}
	
	/* the process that is to be loded into memory */
	int32_t loded_pid;

	/* load... er, lode, the program by copying into user memory */
	loded_pid = loder(dentry.inode_idx);
	if ( loded_pid == -1 )
	{
		printf("I crapped in loder\n");
		return -1;
	}

	/* after launching all 3 shells... */

	/* do the context switch and begin execution */
	int32_t ret = switch_to_process( loded_pid );
	/* returns here when the program halts */
	//printf( "%d\n", ret );
	return ret;
}

/*
 *	Name: loder (Yes, it's misspelled. It's better this way.)
 *	Summary: copies a program image into contiguous memory for execution
 *  Inputs: the index of the inode that holds the data
 *	Outputs: none
 *	Return Value: returns success 0 (loded) or failure -1
 */
int32_t loder( uint32_t inode_idx )
{
	/* Increment number of running processes and switch cr3 to new dir */
	NUM_PROC++;
	int32_t new_pid = get_new_pid();
	cr3_switch(&(pdir[new_pid]));

	/* set the file start to the first block and read four bytes for the start address */
	uint8_t buf[4];
	if ( -1 == read_data( inode_idx, ELF_START, buf, ELF_START_ADDR ) )
		return -1;

	/* get eip for the user */
	uint32_t program_start;
	get_dword_from_bytes(program_start, buf); // dat macro

	/* get the inode pointer so that we can load the file into memory */
	inode_t * inode = get_inode_ptr(inode_idx);

	/* copy program into contiguous memory */
	int catch = read_data( (int32_t)inode_idx, 0, (uint8_t*)PROGRAM_START, inode->length );
	if(catch == -1)
	{
		/* well, shit... abandon ship */
		NUM_PROC--;
		if(NUM_PROC < 1)
			NUM_PROC = 1;
		release_pid( new_pid );
		printf( "OH %#!^ loder got an error that execute should have found\n" );

		if ( CURR_TASK != NULL )
			cr3_switch(&(pdir[CURR_TASK->pid]));

		return -1;
	}
	
	/* initialize the pcb with stdin and stdout, as well as other stuff */
	pcb_t * new_task = get_pcb_from_pid( new_pid );
	init_pcb( new_task, CURR_TASK, new_pid, program_start, arg_temp );

	/* set up default actions for signals */ 
	new_task->signal_ops.div_zero = div_zero_default;
	new_task->signal_ops.seg_fault = seg_fault_default;
	new_task->signal_ops.interrupt = interrupt_default;
	new_task->signal_ops.alarm = alarm_default;
	new_task->signal_ops.user = user_default;

//	if ( NUM_PROC == 3 )
//	{
//		new_task = get_pcb_from_pid( 0 );
//	}

	return new_task->pid;
}

/*
 *	Name: read
 *	Summary: read from a file
 *  Inputs: fd -- the file descriptor (which file in the file array?)
 *			buf -- the user buffer to store the file data
 *			nbytes -- number of bytes to copy
 *	Outputs: buf -- file data is stored in this buffer
 *	Return Value: returns the number of bytes read or -1 for error
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	//printf("read\n");

	if ( fd < 0 || fd >= FILE_ARRAY_SIZE || fd == 1 )
	{
		//printf( "error: read: invalid fd\n" );
		return -1;
	}

	if ( buf == NULL )
	{
		//printf( "error: read: user buffer is null\n" );
		return -1;
	}

	if ( file_in_use(fd) == 0 )
	{
		//printf( "error: read: file descriptor already in use\n" );
		return -1;
	}

	/* read */
	return (get_file_array())[fd].ops->read( fd, buf, nbytes );
}

/*
 *	Name: write
 *	Summary: writes to a file
 *  Inputs: fd -- the index in the file array
 *			buf -- the buffer containing the data to write
 *			nbytes -- the number of bytes from the buffer to write
 *	Outputs: none
 *	Return Value: returns the number of bytes written or -1 for error
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	//printf("write\n");
	
	if ( fd < 0 || fd >= FILE_ARRAY_SIZE || fd == 0 )
	{
		//printf( "error: write: invalid fd\n" );
		return -1;
	}

	if ( buf == NULL )
	{
		//printf( "error: write: user buffer is null\n" );
		return -1;
	}

	if ( file_in_use(fd) == 0 )
	{
		//printf( "error: write: file descriptor already in use\n" );
		return -1;
	}

	/* write */
	return (get_file_array())[fd].ops->write( fd, buf, nbytes );
}

/*
 *	Name: open
 *	Summary: allocates a space in the file array and associates the file with a set of file operations
 *  Inputs: filename -- string containing the name of the file
 *	Outputs: nope
 *	Return Value: returns success 0 or failure -1
 */
int32_t open(const uint8_t* filename)
{
	if ( filename == NULL ) return -1;
	
	//printf("open\n");
	/* find directory entry corresponding to the named file */
	dentry_t dentry;
	if ( read_dentry_by_name( filename, &dentry ) == -1 )
	{
		//printf( "error: open: read_dentry_by_name returned error\n" );
		return -1;
	}

	file_t file;
	/* determine file_type, set up file operations table, and initialize values for file struct */
	switch ( dentry.file_type )
	{
		case TYPE_RTC:
			file.ops = &rtc_ops;
			file.inode_idx = -1;
			file.file_pos = 0;
			break;
		case TYPE_DIR:
			file.ops = &dir_ops;
			file.inode_idx = -1;
			file.file_pos = 0;
			break;
		case TYPE_FILE:
			file.ops = &file_ops;
			file.inode_idx = dentry.inode_idx;
			file.file_pos = 0;
			break;
		default:
			//printf( "error: open: invalid filetype\n" );
			return -1;
	}

	pcb_t *current_pcb = get_current_pcb();
	
	/* attempt to open the driver */
	if ( file.ops->open(filename) == -1 )
	{
		//printf( "error: open: could not open %s\n", filename );
		return -1;
	}

	/* attempt to add the file to the pcb file array */
	int32_t fd;
	if ( (fd = add_file(current_pcb, file)) == -1 )
	{
		//printf( "error: open: could not add file to file array\n" );
		return -1;
	}

	/* successfully opened, return the file index */
	return fd;
}

/*
 *	Name: close
 *	Summary: closes a file in the file array to make it available for another file
 *  Inputs: fd -- the index of the file in the file array to close
 *	Outputs: nope
 *	Return Value: returns success 0 or failure -1 
 */
int32_t close(int32_t fd)
{
	if ( fd < 0 || fd >= FILE_ARRAY_SIZE )
	{
		//printf( "error: close: invalid file descriptor\n" );
		return -1;
	}
	/* can't close stdin or stdout */
	if ( fd == 0 || fd == 1 )
	{
		//printf( "error: close: cannot close stdin or stdout\n" );
		return -1;
	}

	/* can't close a file that isn't in use */
	if ( file_in_use( fd ) == 0 )
	{
		//printf("error: close: file not in use, cannot close\n" );
		return -1;
	}

	/* get the file in the file array */
	file_t file = (get_file_array())[fd];

	/* is the order right or does it matter? */
	if ( file.ops->close( fd ) == 0) // arg for close doesn't matter
	{
		if ( close_file( get_current_pcb(), fd ) == -1 )
		{
			//printf("error: close: could not close file in file array\n");
			return -1;
		}
	}
	else
	{
		//printf("error: close: could not close file\n");
		return -1;
	}

	return 0;
}

/*
 *	Name: getargs
 *	Summary: gets the arguments from the pcb to be used by the user
 *  Inputs: buf -- the user buffer to put the arguments into
 *			nbytes -- number of bytes to copy into buffer (buf)
 *	Outputs: nothin'
 *	Return Value: returns success 0 or failure -1
 */
int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	if ( buf == NULL ) return -1;

	if(get_current_pcb()->arg == NULL)
	{
		//printf("error: getargs: arg in pcb is somehow NULL\n");
		return -1;
	}
	strcpy((int8_t *)buf, (int8_t *)(get_current_pcb()->arg));
	return 0;
}

/*
 *	Name: vidmap
 *	Summary: maps text-mode video memory into user space
 *  Inputs: screen start -- pointer to address of video memory
 *	Outputs: screen start -- points to video memory
 *	Return Value: returns 0 for success and -1 for failure
 */
int32_t vidmap(uint8_t** screen_start)
{
	if ( screen_start == NULL ) return -1;

	if ((screen_start > (uint8_t **)USER_START_PAGE && screen_start<(uint8_t **)USER_END_PAGE) )
	{
		*screen_start = (uint8_t *)VIDEO_MEM;
		return 0;
	}
	else return -1;
}

/*
 *	Name: set_handler
 *	Summary: 
 *  Inputs: 
 *	Outputs: 
 *	Return Value: 
 */
int32_t set_handler(int32_t signum, void * handler_address)
{
	printf("set_handler\n");
	/* signals should only be delivered to a task when returning to user space from the kernel */
	
	pcb_t * pcb = get_current_pcb();

	switch ( signum )
	{
		/* DIV_ZERO sent when divide by zero exception occurs */
		case DIV_ZERO:
			/*
			if ( handler_address == NULL )
				pcb->signal_ops.div_zero = div_zero_default;
			else
				pcb->signal_ops.div_zero = handler_address;
			*/
			pcb->signal_ops.seg_fault = ( handler_address == NULL ? div_zero_default : handler_address );
			break;
		/* SEGFAULT should be sent when any other exception occurs */
		case SEGFAULT:
			/*
			if ( handler_address == NULL )
				pcb->signal_ops.seg_fault = 
			else
				pcb->signal_ops.seg_fault = handler_address;
			*/
			pcb->signal_ops.seg_fault = ( handler_address == NULL ? seg_fault_default : handler_address );
			break;
		/* INTERRUPT should be sent when CTRL+C is pressed */
		case INTERRUPT:
			pcb->signal_ops.seg_fault = ( handler_address == NULL ? interrupt_default : handler_address );
			break;
		/* ALARM signal sent to currently executing task every 10 seconds */
		case ALARM:
			pcb->signal_ops.seg_fault = ( handler_address == NULL ? alarm_default : handler_address );
			break;
		/* Anything you can imagine! */
		case USER1:
			pcb->signal_ops.seg_fault = ( handler_address == NULL ? user_default : handler_address );
			break;
		/* WAT */
		default:
			printf("error: set_handler: invalid signal number - wtf are you doing?\n");
			break;
	}

	return 0;
}

/*
 *	Name: sigreturn
 *	Summary: 
 *  Inputs: 
 *	Outputs: 
 *	Return Value: 
 */
int32_t sigreturn()
{
	printf("sigreturn\n");
	return 0;
}


/****************************/
/***** HELPER FUNCTIONS *****/
/****************************/

/*
 *	Name: get_current_pcb
 *	Summary: gets the process control block of the currently running process
 *  Inputs: nothin'
 *	Outputs: nein
 *	Return Value: a pointer to the current process's pcb
 */
pcb_t * get_current_pcb(void)
{
	int32_t pcb_addr;
	asm volatile(
		"MOVL	%1, %%ebx;"
		"ANDL	%%esp, %%ebx;"
		"MOVL	%%ebx, %0;"
		: "=r" (pcb_addr)
		: "r" (PCB_MASK)
		: "%ebx"
	);

	return (pcb_t*)pcb_addr;
}

/*
 *	Name: get_pcb_from_pid
 *	Summary: gets the process control block with specified pid
 *  Inputs: nothin'
 *	Outputs: nein
 *	Return Value: a pointer to the process's pcb
 */
pcb_t * get_pcb_from_pid(int32_t pid)
{
	return (pcb_t*)(KERNEL_END - (pid+1) * PAGE_SIZE_4KB*2);
}

/*
 *	Name: get_file_array
 *	Summary: gets a pointer to the file array associated with the current pcb
 *  Inputs: nothin'
 *	Outputs: nope
 *	Return Value: returns a pointer to the file array
 */
file_t * get_file_array(void)
{
	pcb_t * current_pcb = get_current_pcb();
	return (current_pcb->file_array);
}

/*
 *	Name: file_in_use
 *	Summary: checks if a file at specified index (fd) is currently being used by the process
 *  Inputs: fd -- the index of the file in the file array
 *	Outputs: none
 *	Return Value: returns 1 (in use) or 0 (not in use)
 */
int32_t file_in_use( int32_t fd )
{
	return (get_file_array())[fd].flags & FLAGS_IN_USE;
}


/*
 *	Name: get_new_pid
 *	Summary: gets a new pid number that is not used for a new active process
 *  Inputs: nothin'
 *	Outputs: nope
 *	Return Value: returns 0 for success or -1 for failure
 */
int32_t get_new_pid( )
{
	int i;
	for ( i = 0; i < MAX_PROC; i++ )
	{
		if ( processes[i] == 0 )
		{
			processes[i] = 1;
			return i;
		}
	}
	return -1;
}

/*
 *	Name: release_pid
 *	Summary: removes an active process
 *  Inputs: nothin'
 *	Outputs: nope
 *	Return Value: returns 0 for success or -1 for failure
 */
int32_t release_pid( int32_t pid )
{
	if ( processes[pid] == 0 )
	{
		printf( "release: wtf this pid isn't taken\n" );
		return -1;
	}

	processes[pid] = 0;
	return 0;
}

/* context switches to another process */
/*	Name: switch_to_process
 *	Summary: transfers control of CPU to a user program using assembly
 *  Inputs: program_start -- the address to begin execution
 *			retval -- the value to return through IRET
 *	Outputs: none
 *	Return Value: retval
 */
int32_t switch_to_process( int32_t pid )
{
	int32_t retval = 0;

	pcb_t * new_task = get_pcb_from_pid( pid );

	// kernel_stack_for_proc is a macro
	tss.esp0 = kernel_stack_for_proc( pid );

	if ( new_task->parent != NULL )
	{
		pcb_t * parent_task = new_task->parent;
		/* save the stack pointer and frame pointer to restore the stack when the program is done */
		asm volatile(
			"MOVL	%%esp, %0			;"
			"MOVL	%%ebp, %1			;"
			: "=r" (parent_task->switch_ESP), "=r" (parent_task->switch_EBP)
		);
	}

	/* change CURR_TASK */
	CURR_TASK = new_task;

	/* push on the hardware context thing */
	/*
		what it looks like:

		|	EIP		| <-- esp (before IRET)
		|	CS		|
		|	EFLAGS	|
		|	ESP		|
		|	SS		|
	*/

	asm volatile(
		"PUSHL	%0					;" // SS
		"PUSHL	%1					;" // ESP
		"PUSHFL						;" // EFLAGS
		"POPL	%%eax				;" // modify EFLAGS
		"ORL	$0x200, %%eax		;" // enable IF flag for interrupts
		"PUSHL	%%eax				;"
		"PUSHL	%2					;" // CS
		"PUSHL	%3					;" // EIP
		"IRET						;"
		:
		: "r" (USER_DS), "r" (USER_END_PAGE), "r" (USER_CS), "r" (CURR_TASK->program_start)
		: "%eax"
	);

	/* WHEN PROGRAM IS DONE */
	/* (CURR_TASK is now parent) */
	asm volatile(
		"halt_ret:					;"
		"MOVL	%%eax, %0			;"
		: "=r" (retval)
		:
		: "%eax"
	);

	return retval;
}
