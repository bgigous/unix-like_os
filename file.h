/* describes a file structure */

#ifndef FILE_H
#define FILE_H

#include "filesys_struct.h"

/* declare function pointer types these will point to the 
	functions for a particular type of file */ 
typedef int32_t (*f_open)(const uint8_t*);
typedef int32_t (*f_read)(int32_t, void*, int32_t);
typedef int32_t (*f_write)(int32_t, const void*, int32_t);
typedef int32_t (*f_close)(int32_t);

/* create a structure for a "jumptable" 
	kind of hard to make an actual jumptable if 
	the functions have different params, right? 
	Just making sure I'm not crazy (Brandon) */ 
typedef struct f_ops {
	f_open open;
	f_read read;
	f_write write;
	f_close close;
} f_ops; //not exactly a "type"

/* a file consists of a jump table for I/O operations 
	a pointer to its inode the position in the file
		where user is currently reading 
	and flags for doing things like indicating status ("in-use") */ 
typedef struct file {
	f_ops * ops;
	int32_t inode_idx;
	int32_t file_pos;
	uint32_t flags;
} file_t;

#endif
