/* keyboard.c - keyboard functions for functioning of the keyboard keys
 * group04
 * spring 2014
 * ece 391
 */

#include "i8259.h"
#include "keyboard.h"
#include "lib.h"
#include "term_driver.h"
#include "sys_handler.h"
#include "terminals.h"

#define TERMINAL_WIDTH 80

int32_t term_switch;
static int full_buff;
static int k;
volatile int id = 2;
volatile uint32_t buff_sizes[3] = {0, 0, 0};
/* keyboard lookup table */
unsigned char default_keys[CHAR_BUF] = 
{	' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'-', '=', ' ', ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n', ' ', 'a', 's', 'd', 'f', 'g', 'h',
	'j', 'k', 'l', ';', '\'', '`', ' ', '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' '
};

unsigned char shift_keys[CHAR_BUF] = 
{	' ', ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
	'_', '+', ' ', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '{', '}', '\n', ' ', 'A', 'S', 'D', 'F', 'G', 'H',
	'J', 'K', 'L', ':', '"', '~', ' ', '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' '
};

/*
* void handle_key(void);
*   Inputs: void
*   Return Value: none
*	Function: Handle a new keyboard interrupt and convert it to ASCII
*/
unsigned char handle_key()
{
	/* handle a key function
	 * show it on the screen */
	int *spec_buffers = special_buffers[id];
	unsigned char scan_code = inb( KEYPORT ); 	// read the port to obtain the key pressed
	switch(scan_code)
	{
		//backspace down
		case BK:
			return bksp;
		//L
		case L:
			if(spec_buffers[0])
			{	
				return ctrl_l;
			}
			break;

		case C:
			if(spec_buffers[0])
			{
				// dat signal
				return 0x01;
			}
			else
				break;
		case F1:
			if(spec_buffers[1] == 1)
			{
				printf("Switch to shell 1\n");
				return f_1;
			}
			return '\0';
		case F2:
			if(spec_buffers[1] == 1)
			{
				printf("Switch to shell 2\n");
				return f_2;
			}
			return '\0';
		case F3:
			if(spec_buffers[1] == 1)
			{
				printf("Switch to shell 3\n");
				return f_3;
			}
			return '\0';
		//space
		case SPACE:
			return ' ';
		//enter
		case ENTER:
			return '\n';
		//tab
		case TAB:
			tab_handle();
			return '\0';		
		//ctrl on
		case CTRL_ON:
			spec_buffers[0] = 1;
			return '\0';			
		//ctrl off
		case CTRL_OFF:
			spec_buffers[0] = 0;
			return '\0';
		//left shift on
		case LSHIFT_ON:
			spec_buffers[2] = 1;
			return '\0';
		//left shift off
		case LSHIFT_OFF:
			spec_buffers[2] = 0;
			return '\0';
		//right shift on
		case RSHIFT_ON:
			spec_buffers[2] = 1;
			return '\0';
		//right shift off 
		case RSHIFT_OFF:
			spec_buffers[2] = 0;
			return '\0';
		//alt on
		case ALT_ON:
			spec_buffers[1] = 1;
			return '\0';
		//alt off
		case ALT_OFF:
			spec_buffers[1] = 0;
			return '\0';
		//caps lock
		case CAPS_LOCK:
			spec_buffers[4] = !(spec_buffers[4]);
			return '\0';
		//num lock
		case NUM_LOCK:
			spec_buffers[3] = !(spec_buffers[3]);
			return '\0';
		//scroll lock
		case SCROLL_LOCK:
			spec_buffers[5] = !(spec_buffers[5]);
			return '\0';
	}
	if((scan_code > 0x01 && scan_code < 0x0E) || (scan_code > 0x19 && scan_code < 0x1C) || (scan_code > 0x26 && scan_code < 0x2C) || (scan_code > 0x32 && scan_code < 0x36))
	{
		if(spec_buffers[2])
			return shift_keys[scan_code];
		else
			return default_keys[scan_code];
	}
	else if((scan_code > 0x0F && scan_code < 0x1A) || (scan_code > 0x1D && scan_code < 0x27) || (scan_code > 0x2B && scan_code < 0x33))
	{
		if(spec_buffers[2] || spec_buffers[4])
			return shift_keys[scan_code];
		else
			return default_keys[scan_code];
	}
	return '\0';
}

/*
* void overlap(void);
*   Inputs: void
*   Return Value: none
*	Function: Add to the keyboard buffer and write to the screen
*/
void handle_press(){
	//int i;
	uint8_t *keyboard_buf = keyboard_buffers[id];
	unsigned long flags;
	cli_and_save(flags);
	//get the new key
	unsigned char pressed = handle_key(id);
	if(pressed == f_1)
	{
		// ALT+F1, switch to terminal 1
		if ( id == 0 ) return; // We're already in terminal 1!
		else id = 0;
		term_switch = 0;
		terminal_switch(0);
		return;
	}
	else if(pressed == f_2)
	{
		// ALT+F2, switch to terminal 2
		if ( id == 1 ) return; // We're already in terminal 2!
		else id = 1;
		term_switch = 1;
		terminal_switch(1);
		return;
	}
	else if(pressed	== f_3)
	{
		// ALT+F3, switch to terminal 3
		if ( id == 2 ) return; // We're already in terminal 3!
		else id = 2;
		term_switch = 2;
		terminal_switch(2);
		return;
	}
	if(pressed == 0x01)
	{
		restore_flags(flags);
		get_current_pcb()->signal_ops.interrupt();
		return;
	}
	//if buffer full already, don't do anything
	if(!full_buff && pressed != '\0')
	{
		//add new key to the buffer
		keyboard_buf[buff_sizes[id]] = pressed;
		//increment size
		buff_sizes[id] += 1;
		//line_size += 1;
	}
	//if buffer full and not entering or clearing the screen, just leave
	else if(full_buff && pressed != '\n' && pressed != ctrl_l && pressed != bksp)
	{
		restore_flags(flags);
		return;
	}
	
	if(buff_sizes[id] >= BUFF_LIM)
		full_buff = 1;
		
	//write the buffer to the screen
	//term_write(keyboard_buf, buff_sizes[id]);
	
	//if backspace, start clearing buffer
	if(pressed == bksp)
	{
		if(buff_sizes[id] < 2)
		{
			keyboard_buf[0] = '\0';
			buff_sizes[id] = 0;
		}
		else
		{
			keyboard_buf[buff_sizes[id]] = '\0';
			keyboard_buf[buff_sizes[id]-1] = '\0';
			buff_sizes[id] -=2;
		}
		if(full_buff && keyboard_buf[BUFF_LIM-1] != bksp)
		{
			buff_sizes[id] += 1;
		}
		full_buff = 0;
		//term_write(keyboard_buf, buff_sizes[id]);
	}
	
	//CTRL-L
	if(pressed == ctrl_l)
	{
		if(buff_sizes[id] > 0 && (buff_sizes[id] != BUFF_LIM || (keyboard_buf[buff_sizes[id]-1] == ctrl_l)))
			buff_sizes[id] -= 1;
		reset_cursor(0);
		update_cursor(0,0);
		//int i;
		/* show the current buffer on the screen */
		/*
		for(i = 0; i < buff_sizes[id]; i++)
		{
			putc(keyboard_buf[i]);
			if(i == TERMINAL_WIDTH)
				inc_screen_row();
		}
		*/
	}
	//get previous column and row info to return to
	k = (get_screen_col());
	int save_row = get_screen_row();
	int row_flag = 0;
	int update_flag = 0;
	int i;
	for(i = 0 ; i < buff_sizes[id]; i++)
	{
		if(keyboard_buf[i] == '\n')
		{
			k = -1;
			//update_flag = 1;
		}
		if(get_screen_col() == TERMINAL_WIDTH-1)
		{
			inc_screen_row();
			k = 0;
			row_flag = 1;
			if(get_screen_row() == NUM_ROWS)
			{
				inc_screen_row();
				row_flag = 0;
			}
		}
		putc(keyboard_buf[i]);
	}
	if(pressed == bksp)
	{
		if(get_screen_col() == 0 && buff_sizes[id] > 0)
		{
			clear_line();
			dec_screen_row();
			clear_line();
			inc_screen_row();
			//dec_screen_row();
			set_screen_col(TERMINAL_WIDTH);
		}
		else
			clear_spot(get_screen_col(), get_screen_row(), 1);
	}
	update_cursor(get_screen_row(), get_screen_col());
	if(get_screen_row() == NUM_ROWS || (update_flag) == 1 || get_screen_col() >= NUM_COLS)
		update_screen();
	if(row_flag)
		set_screen_row(save_row);
	if(k != -1)
		set_screen_col(k);
	if(buff_sizes[id] < BUFF_LIM)
		full_buff = 0;
	restore_flags(flags);
}

//tab handler

/*
* INPUT: 
* OUTPUT: 
* SIDE EFFECTS: completes the keyboard buffer if only 1 choice left
*/
void tab_handle()
{	
	uint8_t *keyboard_buf = keyboard_buffers[id];
	int space_count = 0; // to get the position without a space
	int save_row = get_screen_row();
	int save_col = get_screen_col();
	uint8_t dir_buf[32][32];
	int no_of_files = 0;
	char substring[32];
	int bytes_read;
	int count_printed = 0;
	int32_t t;
	int temp;
	int i, j, k;
	char string[32];
	char fill_string[32];
	int print_count = 0;
	term_write(1, "\n", 1);
	


	clear_line();
	inc_screen_row();
	clear_line();
	inc_screen_row();
	clear_line();
	

	dec_screen_row();
	dec_screen_row();
	
	// did this cause somehow backspace was'nt setting the NULL character
	keyboard_buf[buff_sizes[id]] = '\0';
	for (i=0;i<32;i++){substring[i] = '\0';}
	//check for a space
	for (i=0; keyboard_buf[i]!='\0' && keyboard_buf[i]!=' '; i++){}
	if (i == strlen((int8_t *)keyboard_buf))
	{
		space_count = -1;
		strncpy(substring, (int8_t *)keyboard_buf, strlen((int8_t *)keyboard_buf));
	}	
	else
	{
			while(keyboard_buf[i] == ' ') {i++;}
			space_count = i;
			for ( i =0; keyboard_buf[space_count]!='\0'; space_count++,i++)
			{
				substring[i] = keyboard_buf[space_count];
			}
			substring[i] = '\0';
	}

	for (i = 0;i<32;i++)
	{
		fill_string[i] = '\0';	
	}
	
	
	while( (bytes_read = dir_read(0,(void *)(&dir_buf[no_of_files]),32)) != 0 )
	{
		
		for (i = 0;i<32;i++)
		{
			string[i] = '\0';
		}
		
		if (print_count > 5)
		{
			term_write(1, "\n", 1);
			print_count = 0;
		}	

		dir_buf[no_of_files][bytes_read] = '\0';
		for (i=0; i<=bytes_read; i++)
		{
			string[i] = (char)dir_buf[no_of_files][i];
		}
		if (keyboard_buf[0] == '\0')
		{
					term_write(0,string, bytes_read);
					term_write(0,"   ", 1);
					print_count++;
		}
		else{
		t = (str_find(string,(char *)substring));
		//temp = (strstr(string,(char *)keyboard_buf));
		if ((int)(t) != -1)
		{
			term_write(1, string, strlen(string));
			term_write(1, "   ", 3);
			strncpy(fill_string, string, strlen(string));
			print_count++;
			count_printed +=1;
		}
	
		no_of_files++;
		}      
	
	}
	if (count_printed == 1)
	{

		//find a space in the keyboard_buf
		if (space_count != -1)
		{
			for (j=0; keyboard_buf[j]!=' '; j++);
			while (keyboard_buf[j] == ' '){ j++;}
			//strncpy needs part of fill string to copy(start of fill_string)- j
			//it also needs no of bytes- j+ and starting location of keyboard buf
			for (k=0; substring[k]!= '\0'; k++);
			//so i now holds the len
			temp = strlen((int8_t *)fill_string);
			strncpy((int8_t *)&keyboard_buf[j+k], &fill_string[k], strlen((int8_t *)fill_string) - strlen((int8_t *)substring));
			buff_sizes[id] += (strlen((int8_t *)fill_string) - strlen((int8_t *)substring));
		}
		else
		{
			i=strlen((int8_t *)substring);
		
			temp = strlen((int8_t *)fill_string);
			strncpy((int8_t *)&keyboard_buf[i],&fill_string[i],temp-i);
			buff_sizes[id] +=temp-i;
		}
	}
	set_screen_row(save_row);
	set_screen_col(save_col);
	term_write(1, keyboard_buf, buff_sizes[id]);
	set_screen_row(save_row);
	set_screen_col(save_col);
	//update_cursor(save_row, save_col);
	//if (strstr(keyboard_buf, ))
}

/*
void left_right_handle(int32_t pick)
{
	if(pick == 0)
	{
		int end_col = get_screen_col() + buff_sizes[id] - 1;
		buff_sizes[id] -= 1;
		keyboard_buf[buff_sizes[id]] = '\0';
		update_cursor(get_screen_row(), end_col);
		//set_screen_col(end_col);
		//set_screen_col(get_screen_col()-1);
	}
	if(pick == 1)
	{
		int end_col = get_screen_col() + buff_sizes[id] + 1;
		buff_sizes[id] -= 1;
		keyboard_buf[buff_sizes[id]] = '\0';
		update_cursor(get_screen_row(), end_col);
	}
}
*/
