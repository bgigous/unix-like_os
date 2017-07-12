/* 
	ECE391 - SPR '14
	03/26/2014
	group 04
	initiated and started by rsngrnt2
	
	FILENAME - filesys.c
	PURPOSE - to implement filesystem driver and calls.
*/

#include "filesys.h"
#include "lib.h"
#include "paging.h"
#include "pcb.h"
#include "sys_handler.h"

/* the boot block we'll use to keep track of our file system */
static boot_block_t bb;

/* pointer to the beginning of the inodes */
static inode_t * inodes_ptr;
/* pointer to the beginning of the data blocks */
static data_block_t * data_ptr;
//static count=0; //for the dir_read_offset 
//static uint32_t file_read_offset = 0;
//static uint32_t dir_read_offset = 0;

/*
*	Name: init_filesys
*	Summary: sets up the data structures with information about the files
*   Inputs: a pointer to the beginning of the file system
*	Outputs: none
*	Return Value: returns success 0 or failure -1
*/
int32_t init_filesys( uint32_t * filesystem_ptr )
{
	if ( filesystem_ptr == NULL )
		return -1;

	/* get only the directory entry filename */
	_dentry_name_t * _dn = (_dentry_name_t*)(filesystem_ptr + DIR_ENTRIES_OFFSET);
	/* get only the directory entry fields */
	_dentry_fields_t * _df = (_dentry_fields_t*)(filesystem_ptr + DIR_ENTRIES_OFFSET);

	/* get only the boot block fields */
	_bbf_t * _bbf = (_bbf_t*)filesystem_ptr;

	/* start filling in the boot block structure */
	bb.num_dir_entries = _bbf->num_dir_entries;
	bb.num_inodes = _bbf->num_inodes;
	bb.num_data_blocks = _bbf->num_data_blocks;

	int e; //entry iterator
	for ( e = 0; e < _bbf->num_dir_entries; e++ )
	{
		/* fill in each directory entry to the boot block structure */
		strcpy( (int8_t*)&(bb.entries[e].file_name), (int8_t*)&(_dn[e].file_name) );

		bb.entries[e].file_type = _df[e].file_type;
		bb.entries[e].inode_idx = _df[e].inode_idx;

	}

	/* find the address of where the inodes start */
	inodes_ptr = (inode_t*)(_bbf + 1);
	/* find the address of where the data starts */
	data_ptr = (data_block_t*)(inodes_ptr + bb.num_inodes);

	return 0; /* success! */
};

/*
*	Name: read_dentry_by_name
*	Summary: fills in a directory entry data structure with information about the file
*				with the given filename
*   Inputs: fname -- string that represents the filename
*			dentry -- the directory entry data structure
*	Outputs: dentry -- contains information about the directory entry
*	Return Value: success 0 or failure -1
*/
int32_t read_dentry_by_name( const uint8_t * fname, dentry_t * dentry )
{
	if ( fname == NULL || dentry == NULL )
		return -1;

	int e; // directory entry iterator
	for ( e = 0; e < bb.num_dir_entries; e++ )
	{
		// compare the first 32 characters and see if they are the same
		// if they are, strncmp returns 0
		if ( !strncmp( (int8_t*)bb.entries[e].file_name, (int8_t*)fname, FILE_NAME_LENGTH - 1 ) )
		{
			*dentry = bb.entries[e];
			return 0;
		}
	}

	/* could not find file */
	return -1;
}

/*
*	Name: read_dentry_by_index
*	Summary: fills in a directory entry data structure with information about the file
*				with the given directory index
*   Inputs: index -- the index of the file in the directory
*			dentry -- the directory entry data structure
*	Outputs: dentry -- contains the information about the directory entry
*	Return Value: success 0 or failure -1
*/
int32_t read_dentry_by_index( uint32_t index, dentry_t * dentry )
{
	if ( dentry == NULL )
		return -1;

	/* index can't be greater than or equal to the number of entries */
	if ( index >= bb.num_dir_entries )
	{
		return -1;
	}
	else
	{
		*dentry = bb.entries[index];
		return 0;
	}
}

/*
*	Name: read_data
*	Summary: reads the data of a file byte by byte and copies the data into a buffer
*   Inputs: inode -- the index of the inode (represents the file)
*			offset -- the offset from the beginning of the file in bytes
*			buf -- the buffer to copy the bytes into
*			length -- the number of bytes to copy
*	Outputs: buf -- the file data
*	Return Value: success 0 or failure -1
*/
int32_t read_data( uint32_t inode_idx, uint32_t offset, uint8_t * buf, uint32_t length )
{
	if ( buf == NULL )
		return -1;

	/* check for invalid inode index */
	if ( inode_idx >= bb.num_inodes ) return -1;

	inode_t the_inode = inodes_ptr[inode_idx];

	/* len is the total number of bytes in the file */
	int len = the_inode.length;

	/* b is our byte iterator */
	int b = 0;

	int db = the_inode.dblock_nums[which_data_block(b + offset)];
	int byte_pos = data_block_byte(b + offset);

	/* until we read all the bytes we want 
		or we can't read any more bytes (EOF)	*/
	// NOTE: the && means that both conditions must be satisfied
	while ( b < length &&
			b + offset < len )
	{
		/* db indicates which data block we should find the data */
		db = the_inode.dblock_nums[which_data_block(b + offset)];
		/* check if data block is valid */
		if ( db >= bb.num_data_blocks )
			return -1;
		/* byte_pos tells us which byte is the next byte we want */
		byte_pos = data_block_byte(b + offset);

		*buf = data_ptr[db].data[byte_pos];

		buf++;
		b++;
	}
	
	return 0;
}

/*
*	Name:  file_open
*	Summary: Opens a file for other operations
*   Inputs: file_name - the file name
*	Outputs: 	return value (int32_t)
*	Return Value: -1 if file not found 0 if successful
*/
int32_t file_open( const uint8_t * filename )
{
	dentry_t dentry;
	/* returns the success or failure of finding the file */
	return read_dentry_by_name( filename, &dentry );
}

/*
*	Name:  dir_open
*	Summary: Opens a directory for other operations
*   Inputs: dir_name - the directory name
*	Outputs: 	return value (int32_t)
*	Return Value: -1 if file not found 0 if successful
*/
int32_t dir_open( const uint8_t * filename )
{
	/* . is the only folder */
	/* returns 1 if strncmp returns 0 (filename matches .) */
	return !strncmp( (int8_t*)filename, (int8_t*)".", FILE_NAME_LENGTH );
}

/*
*	Name: file_read
*	Summary: read a file from the beginning and put number of bytes into buffer
*   Inputs:  	inode_idx - the index of the inode of the file
					buf - buffer to write to
					nbytes - number of bytes to write
*	Outputs: 	buf - the data from the file 
*	Return Value:  the number of bytes read from the file
*/
int32_t file_read( int32_t fd, void *buf, int32_t nbytes ) 
{	
	if ( buf == NULL )
		return -1;

	file_t * file = &(get_file_array())[fd];
	int32_t inode_idx = file->inode_idx;

	int32_t file_offset = file->file_pos;

	inode_t * inode = get_inode_ptr( inode_idx );

	// if we can't read anymore, return 0
	if ( file_offset >= inode->length )
	{
		return 0;
	}

	// calculate number of bytes to read
	int32_t bytes = bytes_to_read( file_offset, nbytes, inode->length );
	
	// if read_data successful
	if ( !read_data( inode_idx, file_offset, buf, bytes ) )
	{
		// update file_position
		file->file_pos += bytes;
		return bytes;
	}
	else return -1;
}

/*
	dir_read
	Inputs: buf having the file name
	Output: print the directory file names
*/
int32_t dir_read( int32_t fd, void* buf, int32_t nbytes )
{
	if ( buf == NULL )
		return -1;

	file_t * file = &(get_file_array())[fd];
	int32_t index_offset = file->file_pos;

	dentry_t dentry;
	/* get the directory entry at our index */
	int ret_read_by_index = read_dentry_by_index(index_offset, &dentry);
	if (ret_read_by_index == -1)
	{
		/* can't return here, we haven't checke if process has read the entire directory */
		//return -1;
	}

	/* can't have an index as big or bigger than the number of entries */
	/* this tells the process that we've read all the entries */
	if ( index_offset >= bb.num_dir_entries )
	{
		file->file_pos = 0;
		return 0;
	}	
	
	int i;
	for (i=0; i <= FILE_NAME_LENGTH && (unsigned char )dentry.file_name[i] != '\0'; i++)
	{
		((uint8_t *)buf)[i] = (uint8_t)dentry.file_name[i];
	}
	file->file_pos += 1;
	return i;
}

/*
*	Name: file_write
*	Summary: basically does nothing since it's a read only file system
*   Inputs: they aren't worth mentioning; they do nothing
*	Outputs: none
*	Return Value: -1 (read-only)
*/
int32_t file_write( int32_t fd, const void *buf, int32_t nbytes ) 
{
	/* read only filesystem return -1 */
	return -1 ;
}

/*
*	Name: dir_write
*	Summary: basically does nothing since it's a read only file system
*   Inputs: they aren't worth mentioning; they do nothing
*	Outputs: none
*	Return Value: -1 (read-only)
*/
int32_t dir_write( int32_t fd, const void *buf, int32_t nbytes ) 
{
	return -1 ;
}

/*
*	Name:file_close
*	Summary:  used to close a file
*   Inputs: fd - file descriptor
*	Outputs: none
*	Return Value: -1
*/
int32_t file_close( int32_t fd ) 
{
	return 0 ;
}

/*
*	Name: 
*	Summary: 
*   Inputs: 
*	Outputs: 
*	Return Value: 
*/
int32_t dir_close( int32_t fd ) 
{
	return 0 ;
}



/****************************/
/***** HELPER FUNCTIONS *****/
/****************************/

/*
*	Name: 
*	Summary: 
*   Inputs: 
*	Outputs: 
*	Return Value: 
*/
inode_t * get_inode_ptr( uint32_t inode_idx )
{
	return inodes_ptr + inode_idx;
}

