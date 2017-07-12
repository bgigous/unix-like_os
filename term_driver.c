#include "term_driver.h"
#include "keyboard.h"
#define VIDEO 0xB8000 
#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x7

int screen_loc = 0;
int NUM_COLS_active = 0;

/*
* void term_init(void);
*   Inputs: void
*   Return Value: none
*	Function: Initialize the terminal driver
*/
void term_init(void)
{
	/* initializes the keyboard so that the cpu can receive interrupts from it */
	unsigned long flags;				// used for saving the flags
	cli_and_save(flags);				// clear and store interrupts
	enable_irq(KEYBOARD_IRQ);			//enable the keyboard
	restore_flags(flags);				//restore the flags
	//clear();
	update_cursor(0,0);
}

/*
* int32_t term_open(void);
*   Inputs: void
*   Return Value: 0
*	Function: System call to start terminal drivers
*/
int32_t term_open(const uint8_t* filename)
{
	uint8_t *keyboard_buf = keyboard_buffers[id];
	reset_cursor(0);
	term_init();
	memset(keyboard_buf, '\0', BUFF_LIM);
	buff_sizes[id] = 0;
	//line_size = 0;
	NUM_COLS_active = 0;
	return 0;
}

/*
* int32_t term_write(const void *buf, int32_t nbytes);
*   Inputs: buffer to write and the number of bytes from it to write
*   Return Value: number of bytes written to the screen
*	Function: Writes character buffer to the screen
*/
int32_t term_write(int32_t fd, const void *buf, int32_t nbytes)
{
	//screen_loc = get_screen_row();
	unsigned long flags;			// used to store flags
	cli_and_save(flags);
	//current length is 0, buffer is empty
	//int line_size = 0;
	//unsigned char line_buffer[BUFF_LIM]={'\0'};
    unsigned char *tmp_buf = (unsigned char *)buf;
	int i;//, write_len;
	//nothing to write, then return
    if(buf == NULL)
	{
		restore_flags(flags);				//restore the flags
		return ERR;
	}
	//check if still need to NUM_COLS
    for(i = 0; i < nbytes; i++)
    {
		//put the character to the screen
		 //if need to NUM_COLS
		if(get_screen_col() == NUM_COLS-1 && i != nbytes-1)
		{
			//enable NUM_COLS			
			//go to the next row
			inc_screen_row();
			set_screen_col(0);
			if(get_screen_row() == NUM_ROWS)
			{
				update_screen();
				//screen_loc -= 1;
			}
		}
		putc(tmp_buf[i]);	
		//if newline found
		if(tmp_buf[i] == '\n')
		{
			if(get_screen_row() == NUM_ROWS)
			{
				update_screen();
				//screen_loc -= 1;
			}
			//update the cursor
			update_cursor(screen_loc, 0);
		}
    }
	//update the cursor
	if(buff_sizes[id] < NUM_COLS)
		update_cursor(get_screen_row(), get_screen_col());
	else
		update_cursor(get_screen_row()-1, get_screen_col());
	//restore the flags
	restore_flags(flags);
	//printf("HIIIIIIIIIIIIIIIIIIII");
	return nbytes;
}

/*
* int32_t term_read(int32_t fd, const void *buf, int32_t nbytes);
*   Inputs: buffer to copy data to, number of bytes to copy
*   Return Value: number of bytes copied
*	Function: Fills given buffer
*/
int32_t term_read(int32_t fd, void* buf, int32_t nbytes)
{
	//uint8_t *keyboard_buf = keyboard_buffers[id];
	//unsigned long flags;			// used to store flags
	//cli_and_save(flags);			
	uint32_t ff, should_ret = 0;
	if(buf == NULL)
	{
		ff = -1;
		goto clean_buff;
	}
	while(buff_sizes[id] < nbytes)// && buff_sizes[id] < BUFF_LIM)
	{
		ff = 0;
		unsigned long flags;
		cli_and_save(flags);
		for(ff = 0; ff < nbytes; ff++){
			((char *)buf)[ff] = keyboard_buffers[id][ff];
			if(((char *)buf)[ff] == '\n')
			{
				should_ret = 1;
				break;
			}
			if(ff >= buff_sizes[id])
			{
				//should_ret = 2;
				break;
			}
		}
		restore_flags(flags);
		if(should_ret)
		{
			if(should_ret == 1)
			{
				((char *)buf)[ff] = '\n';
				ff+=1;
			}
		    break;
		}
	}
	//((unsigned char *)buf)[nbytes-1] = '\n';
	//restore_flags( flags);
	//clear the buffer (what's left)
	clean_buff:;
	int k;
	for(k = 0; k < BUFF_LIM; k++)
		keyboard_buffers[id][k] = '\0';
	buff_sizes[id] = 0;
	//buff_sizes[id] -= i;
	//memmove(keyboard_buf, keyboard_buf+i*sizeof(int32_t), buff_sizes[id]);
	return ff;	
}

int32_t term_close(int32_t fd)
{
	//disable_irq(KEYBOARD_IRQ);			//enable the keyboard
	return 0;
}
