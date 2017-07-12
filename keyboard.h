#ifndef _SNS_H
#define _SNS_H

#include "types.h"
#include "lib.h"

#define KEYPORT 		0x60
#define BUFF_LIM 		128
#define KEYBOARD_IRQ	0x01
#define ctrl_l			0x07
#define bksp			0x08
#define CHAR_BUF		0x73
#define SPEC_BUF		6
#define BK				0x0E
#define OVERLAP			80
#define ERR				-1
#define L				0x26
#define C				0x2E
#define F1				0x3B
#define F2				0x3C
#define F3				0x3D
#define f_1				253
#define f_2				254
#define f_3				255
#define SPACE			0x39
#define ENTER			0x1C
#define TAB				0x0F
#define CTRL_ON			0x1D
#define CTRL_OFF		0x9D
#define LSHIFT_ON		0x2A
#define LSHIFT_OFF		0xAA
#define RSHIFT_ON		0x36
#define RSHIFT_OFF		0xB6
#define ALT_ON			0x38
#define ALT_OFF			0xB8
#define CAPS_LOCK		0x3A
#define SCROLL_LOCK		0x46
#define NUM_LOCK		0x45
#define NUM_ROWS 		25
#define NUM_COLS		80
int special_buffers[3][SPEC_BUF];
uint8_t keyboard_buffers[3][128];
extern volatile uint32_t buff_sizes[3];
//uint8_t keyboard_buf[BUFF_LIM];
/*
CTRL	-> [0]
ALT		-> [1]
SHIFT	-> [2]
NUM_LCK	-> [3]
CAP_LCK	-> [4]
SCR_LCK	-> [5]
*/
//int spec_buffers[SPEC_BUF];
//volatile uint32_t buff_size; // changed from uint8_t to uint32_t
//volatile uint8_t line_size;
//functions for enabling the keyboard and handling interrupts
extern volatile int id;
/* tell the PIC to unmask interrupts for the keyboard */

/* handles key presses */
unsigned char handle_key();
void handle_press();
void tab_handle();
//void left_right_handle(int32_t pick);
//int32_t kb_read(void* buf, int32_t nbytes);
#endif

