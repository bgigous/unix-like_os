#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"
#include "filesys_struct.h" /* all the structures for the filesystem are now in here
								because it makes life easier  */

#define TYPE_FILE 		2
#define TYPE_DIR  		1
#define TYPE_RTC		0

/* calculates which data block the data at pos is 
	Example 1:  if we want the 109th (pos = 109) byte of data of a file, 
				we find it in the 0th data block (given by inode)
	Example 2:  if we want the 1053rd (pos = 1053) byte of data,
				we find it in the 1st data block						*/
#define which_data_block(pos)	((pos)/MAX_DATA_BLOCK_ENTRIES)
/* calulates the position of a byte within a data block 
	This would return 29 for Example 2 above							*/
#define data_block_byte(pos)	((pos)%MAX_DATA_BLOCK_ENTRIES)

/* calculates the number of bytes to read from a file given the
	offset in bytes (pos), the number of bytes we WANT to read,
	and the length of the file in bytes 								
	Example: pos is 5, nbytes is 5, length is 9
			bytes_to_read will return 4									*/
#define bytes_to_read(pos,nbytes,length)	\
	( (pos) + (nbytes) > (length) ? (length) - (pos) : (nbytes) )

/* function prototypes */
int32_t init_filesys( uint32_t * filesystem_ptr );

/* fill in a directory entry structure with a file's statistics */
int32_t read_dentry_by_name( const uint8_t * fname, dentry_t * dentry );
int32_t read_dentry_by_index( uint32_t index, dentry_t * dentry );

/* read the data in a file and output it to a buffer */
int32_t read_data( uint32_t inode_idx, uint32_t offset, uint8_t * buf, uint32_t length );

/* file operations */
int32_t file_open( const uint8_t * filename );
int32_t file_read( int32_t fd, void *buf, int32_t nbytes );
int32_t file_write( int32_t fd, const void *buf, int32_t nbytes );
int32_t file_close( int32_t fd ) ;

/* directory operations */
int32_t dir_open( const uint8_t * filename );
int32_t dir_read( int32_t fd, void *buf, int32_t nbytes );
int32_t dir_write( int32_t fd, const void *buf, int32_t nbytes ) ;
int32_t dir_close( int32_t fd );

/* helper functions */
inode_t * get_inode_ptr( uint32_t inode_idx );

#endif /* FILESYS_H */

