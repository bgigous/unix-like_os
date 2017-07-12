#include "pcb.h"

#define STDIN 			0
#define STDOUT			1
#define ARR_LEN 		8
#define IN_USE_MASK		0x00000001
#define ARGUMENT_LENGTH 32
#define MAX_PROC		6

static f_ops stdin_ops 	= { ni_open, term_read, ni_write, ni_close };
static f_ops term_ops 	= { term_open, ni_read, term_write, term_close };

/*
 *	Name: ni_*
 *	Summary: these are for driver specific file opterations that have no defined implementation
 *  Inputs: various
 *	Outputs: none
 *	Return Value: -1
 */
int32_t ni_open(const uint8_t *filename)
{
	return -1;
}
/* see above for info */
int32_t ni_read(int32_t fd, void *buf, int32_t nbytes)
{
	return -1;
}
/* see above for info */
int32_t ni_write(int32_t fd, const void *buf, int32_t nbytes)
{
	return -1;
}
/* see above for info */
int32_t ni_close(int32_t fd)
{
	return -1;
}

/*
 *	Name: init_pcb
 *	Summary: initalizes a process control block; sets up standard input and standard output
 *  Inputs: the pointer to the pcb to initialize
 *	Outputs: none
 *	Return Value: nothing
 */
int32_t init_pcb( pcb_t * pcb, pcb_t * parent, int32_t pid, uint32_t start, uint8_t * arg )
{
	if ( pcb == NULL ) return -1;
	/* set up files for stdin and stdout */
	/* create exception/decoy function for non-existant sys calls? */
	//pcb->arg = NULL;

	if ( pcb->pid != -1 )
	{

	//	printf( "WARNING: pcb already initialized\n" );
	}

	pcb->pid = pid;
	pcb->program_start = start;
	pcb->parent = parent;
	if ( parent != NULL )
		parent->child = pcb;
	pcb->child = NULL;
	// copy in argument
	strncpy( (int8_t*)pcb->arg, (int8_t*)arg, ARGUMENT_LENGTH );

	file_t file_stdin;
	file_stdin.ops = &stdin_ops;
	file_stdin.inode_idx = -1;
	file_stdin.file_pos = 0;
	add_file(pcb, file_stdin);
	file_t file_stdout;
	file_stdout.ops = &term_ops;
	file_stdout.inode_idx = -1;
	file_stdout.file_pos = 0;
	add_file(pcb, file_stdout);
	
	int i;
	for ( i = 2; i < FILE_ARRAY_SIZE; i++ )
		pcb->file_array[i].flags &= ~IN_USE_MASK; // set to "not in use"

	pcb->sig_pending = 0x00;
	pcb->sig_masked = 0x00;

	return 0;
}

/*
 *	Name: add_file
 *	Summary: adds a file to the file array
 *  Inputs: pcb -- a pointer to the pcb containing the file array in question
 *			file -- the file to add
 *	Outputs: nada
 *	Return Value: returns success 0 or failure -1
 */
int32_t add_file(pcb_t *pcb, file_t file)
{
	if ( pcb == NULL )
		return -1;

	int i;
	for(i = 0; i < ARR_LEN; i++)
	{
		if(!(pcb->file_array[i].flags & IN_USE_MASK)){
			/* check if allowed */
			pcb->file_array[i].ops = file.ops;
			pcb->file_array[i].inode_idx = file.inode_idx;
			pcb->file_array[i].file_pos = file.file_pos;
			pcb->file_array[i].flags |= IN_USE_MASK;
			pcb->file_array[i].flags |= file.flags;
			return i;
		}
	}
	return -1;
}

/*
 *	Name: close_file
 *	Summary: closes a file in the file array and frees space for another file
 *  Inputs: pcb -- a pointer to the pcb containing the file array in question
 *			fd -- the index of the file to close
 *	Outputs: none
 *	Return Value: returns success 0 or failure -1
 */
int32_t close_file( pcb_t * pcb, int32_t fd )
{
	if ( pcb == NULL )
		return -1;

	/* That file isn't even in use! What were you thinking?! */
	if ( (pcb->file_array[fd].flags & IN_USE_MASK) == 0 )
		return -1;

	pcb->file_array[fd].ops = NULL;
	pcb->file_array[fd].inode_idx = -1;
	pcb->file_array[fd].file_pos = -1;
	pcb->file_array[fd].flags ^= IN_USE_MASK; //NOT in use

	return 0;
}

/*
 *	Name: close_pcb
 *	Summary: unitializes a pcb
 *  Inputs: pcb -- a pointer to the pcb that we will clear
 *	Outputs: pcb -- cleared
 *	Return Value: returns success 0 or failure -1
 */ 
int32_t close_pcb( pcb_t * pcb )
{
	if ( pcb == NULL )
		return -1;

	pcb->pid = -1;
	pcb->program_start = 0;
	/* the file array (Appendix A 7.2) */
	pcb->parent = NULL;	// the parent task's pcb pointer
	pcb->child = NULL;
	pcb->arg[0] = '\0'; // clear arg
	
	/* close all files */
	int i;
	for ( i = 0; i < FILE_ARRAY_SIZE; i++ )
		close_file( pcb, i );

	return 0;
}

/****************************/
/***** HELPER FUNCTIONS *****/
/****************************/

// empty :c
