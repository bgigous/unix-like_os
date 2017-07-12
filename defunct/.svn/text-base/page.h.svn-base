#ifdef _ASM
#define _ASM
void init_table(void);
void setup_paging(void);
#endif
#ifndef _PAGE_H
#define _PAGE_H

#define PSE_FLAG			0x00000010
#define PAE_FLAG			0xFFFFFFDF
#define PE_FLAG				0x00000001
#define PG_FLAG				0x80000000

#define BASE_ADDR_MASK		0xFFFFF000
#define BASE_ADDR_MASK_4MB	0xFFC00000

#define PRESENT				0x00000001

#define KERNEL_START		0x400000
#define VIDEO 				0xB8000
//void setup_paging(void);
/*
enum pte_flags
{
	pte_present	 		= 0x1,
	pte_writable 		= 0x2,
	pte_user	 		= 0x4,
	pte_writethough		= 0x8, //not sure about this flag if it should be set or not
	pte_not_cahceable	= 0x10,
	pte_accessed		= 0x20,
	pte_dirty			= 0x40,
	pte_pat				= 0x80,
	pte_cpu_global		= 0x100,
	pte_lv4_global		= 0x200,
	pte_frame			= 0x7FFFF000
};

//EACH PAGE DIRECTORY ENTRY
typedef struct {
    union {
        uint32_t address;
        struct {
            uint32_t pde_flags : 12;
            uint32_t pde_addr : 20;
        } __attribute__((packed));
    };
} pd_entry;

//EACH PAGE TABLE ENTRY
typedef struct {
    union {
        uint32_t address;
        struct {
            uint32_t pte_flags : 12;
            uint32_t pte_addr : 20;
        } __attribute__((packed));
    };
} pt_entry;


// number of pages per page table 
#define PAGES_PER_TABLE 		1024
// number of page tables per page directory
#define PAGE_TABLES_PER_DIR		1024



#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

typedef struct ptable_struct{
	pt_entry m_entries[PAGES_PER_TABLE];
} ptable;
typedef struct pdirectory_struct{
	pd_entry m_entries[PAGES_PER_DIR];
} pdirectory;

void enable_paging();
void page_init();
void set_pdi();
void fourkb_pagedir_setup(uint32_t index, uint32_t flags,uint32_t addr);
void fourkb_pagetable_setup(uint32_t index, uint32_t flags,uint32_t addr);
//void load_page();
*/

#endif
