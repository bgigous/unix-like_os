#ifndef FILESYS_STRUCT_H
#define FILESYS_STRUCT_H

#include "types.h"

#define RESERVED_FDE 24
#define RESERVED_BB 52
#define RESERVED_DF1 32
#define RESERVED_DF2 24
#define FILE_NAME_LENGTH 33
#define MAX_DIR_ENTRIES 63 /* 64 - 1 for statistics */
#define MAX_DATA_BLOCK_ENTRIES 4096
#define INODE_DB_MAX 1023

// number of 4 bytes from filesystem pointer
#define DIR_ENTRIES_OFFSET 16

#define DENTRY_SIZE 64

/* the directory entry file name structure
	contains each entry's file name
	[aligned to 64 bytes]
	[don't touch unless you know what you are doing]	*/
typedef struct _dentry_name {
	uint8_t		file_name[FILE_NAME_LENGTH];
	uint8_t		reserved[DENTRY_SIZE - FILE_NAME_LENGTH];
} _dentry_name_t;

/* the directory entry fields structure
	contains info about each entry's file type and inode
	[aligned to 64 bytes] 								
	[don't touch unless you know what you are doing]	*/
typedef struct _dentry_fields {
	uint8_t		reserved1[RESERVED_DF1];
	uint32_t	file_type;
	uint32_t	inode_idx;
	uint8_t		reserved2[RESERVED_DF2];
} _dentry_fields_t;

/* each entry in a directory has a name, type, and an associated inode.
	[this structure is not aligned to any particular size]				*/
typedef struct dentry {
	uint8_t		file_name[FILE_NAME_LENGTH];
	uint32_t	file_type;
	uint32_t	inode_idx;
	uint8_t		reserved[RESERVED_FDE]; /* for silly alignment purposes */
} dentry_t;

/* contains statistics about the current directory
	[aligned to 4096 bytes] 								
	[don't touch unless you know what you are doing]	*/
typedef struct _boot_block_fields {
	uint32_t	num_dir_entries;
	uint32_t	num_inodes;
	uint32_t	num_data_blocks;
	uint8_t		reserved[RESERVED_BB + DENTRY_SIZE*MAX_DIR_ENTRIES];
} _bbf_t;

/* the boot block contains statistics about the filesystem 
	[this structure is not aligned to any particular size]				*/
typedef struct boot_block {
	uint32_t	num_dir_entries;
	uint32_t	num_inodes;
	uint32_t	num_data_blocks;
	uint8_t		reserved[RESERVED_BB];
	dentry_t	entries[MAX_DIR_ENTRIES];
} boot_block_t;

/* inode struct 
	describes a files length in bytes and where the data for the file is*/
typedef struct inode {
	uint32_t length; /* in bytes */
	uint32_t dblock_nums[INODE_DB_MAX];
} inode_t;

typedef struct data_block {
	uint8_t data[MAX_DATA_BLOCK_ENTRIES];
} data_block_t;

#endif
