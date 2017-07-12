#ifndef PAGING_H
#define PAGING_H

#define MEMORY_SIZE 0x1000
#define PAGE_SIZE   0x1000
#define NUM_PAGE    1024
#define NUM_TABLE   1024
#define CURR_PAGES 3
#define KERNEL_ADDR 0x400000
#define PAGE_8MB			0x800000
#define UPPER_20 12
#define four_KB 4096
#define EN_PAGING 0x80000000
#define USER_PAGE 0x00C00000
#define PAGING_PT_ADDR 22
#define PAGING_MASK 0x1FF
#define PAGING_FINAL_BIT 0x1
#define PAGING_RES 13
#define PAGING_PAT 12
#define PAGE_SIZE_4KB 4096
#define INDEX_USER 32
#define NUM_USER_PDIRS 6

/* Made these, we'll put them in after CP3 so I don't mess something up. -Brando */
//#define get_frame(ptable)	((uint32_t) (ptable)) >> 23
//#define get_res(ptable)		((uint32_t) (ptable) >> 13)  & 0x1FF
//#define get_pat(ptable)		((uint32_t) (ptable) >> 12) & 0x1

#include "types.h"

/*
Struct for a single page table entry, indicates the number of bits that will be used
*/
typedef union pt {
	uint32_t val;
	struct{
		uint32_t pres : 1;		// Present
		uint32_t read : 1;		// Read, Write
		uint32_t user : 1;		// User, Supervisor
		uint32_t write : 1;		// Write Through
		uint32_t cache : 1;		// Cache Disabled
		uint32_t access : 1;	// Accessed
		uint32_t dirty : 1;		// dirty
		uint32_t pat : 1;		// Always 0
		uint32_t global : 1;	// global
		uint32_t avl : 3;		// Available Bits
		uint32_t frame : 20;	// 4-KB Aligned Address
	}  __attribute__((aligned(4)));
} pt;

/*
Struct for a single page dierctory entry
*/
typedef union pd {
	uint32_t val;
	struct{
		uint32_t pres : 1;		// Present
		uint32_t read : 1;		// Read, Write
		uint32_t user : 1;		// User, Supervisor
		uint32_t write : 1;		// Write Through
		uint32_t cache : 1;		// Cache Disabled
		uint32_t access : 1;	// Accessed
		uint32_t d : 1;			// Always 0
		uint32_t size : 1;		// Page Size
		uint32_t avl : 4;		// Available Bits
		uint32_t pat : 1;
		uint32_t res : 9;
		uint32_t frame : 10;	// 4-KB Aligned Address
	}  __attribute__((aligned(4)));
} pd;


/*
Actual page table struct
*/
typedef struct pt_t
{
	pt table_entry[NUM_PAGE];
}  __attribute__((aligned(4096))) pt_t;

/*
Actual page directory struct
*/
typedef struct pd_t
{
	pd dir_entry[NUM_TABLE];
}  __attribute__((aligned(4096))) pd_t;

typedef struct pd_d
{
	pd_t pdirs;
	pt_t p_tables[NUM_PAGE];
	int p_addr;
	//stuff
} pd_d;


/* Page directory */
extern pd_t pdir[NUM_USER_PDIRS];
//extern pd_t *pdir;
extern pd_t kernel_directory;

/* Page table for 0MB to 4MB */
extern pt_t p_table_0[NUM_USER_PDIRS];
extern pt_t p_table_0_kernel;
/* Page table for 8 to 12 MB */
extern pt_t p_table_1[NUM_USER_PDIRS];
extern pt_t p_table_1_kernel;

extern pt_t p_tables[NUM_TABLE];
extern pt_t p_tables_pdir1[NUM_TABLE];

extern int NUM_PROC;
//extern pd_t p_dir_tables[NUM_TABLE];
//extern pd_t p_dir_tables_pdir1[NUM_TABLE];
/* Functions to set up and enable paging */
void init_paging(pt_t *p_table_0, pt_t *p_table_1, pd_t *pdir, int idx);
void enable_paging();
uint32_t add_pdir(uint32_t vaddr, int user, int size, pd_t *pdir1, int idx);
void cr3_switch(pd_t *pdir);
#endif
