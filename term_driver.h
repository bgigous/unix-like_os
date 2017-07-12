#ifndef READ_WRITE
#define READ_WRITE
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#define TERM 1
#define KEYB 0
#define KEYPORT 0x60
#define KEYBOARD_IRQ	0x01

int32_t term_open(const uint8_t* filename);
int32_t term_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t term_read(int32_t fd, void *buf, int32_t nbytes);
int32_t term_close(int32_t fd);
#endif
