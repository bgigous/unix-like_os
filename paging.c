#include "paging.h"
#include "lib.h"
#define CURR_PAGES 3
#define PT0_MEM1 0x0800000
#define PT1_MEM1 0x0900000
#define PAGE_OFFSET 0x7D000

//static int idx = 4;
pd_t pdir[6];
//pd_t *pdir_ptr = &pdir;
pt_t  p_table_0[6];
pt_t  p_table_1[6];
pd_t *CURR_PDIR;
int NUM_PROC = 0;
//TEMP?
uint32_t proc_eip[7];
//kernel directory
//pdir =  (pd_t *)0xC800000;
pd_t kernel_directory;
pt_t p_table_0_kernel;
pt_t p_table_1_kernel;
//pt_t p_tables_pdir1[NUM_TABLE];
/*
Purpose: initialize pages of the page directory and page directory contents
Arguments: page table1 , page table2, page directory of the process, argument to denote if it is;
Called: during boot before enabling interrupts
Result: initializes paging (does not turn paging on)
*/

//user flag = 0 then kernel , 1 = user
//size flag = 0 then 4KB, 1 = 4MB


void init_paging(pt_t *page_table_0, pt_t *page_table_1, pd_t *page_dir, int idx)
{
	page_table_0 = (pt_t *)(PT0_MEM1 + (idx*PAGE_OFFSET));
	page_table_1 = (pt_t *)(PT1_MEM1 + (idx*PAGE_OFFSET));
	int i;
	/* Initialize page directory and use flags */
	//printf("%u", ((uint32_t) (&page_table_0)));
	for(i = 0; i < NUM_PAGE; i++)
	{
		page_dir->dir_entry[i].frame = ((uint32_t) (page_table_0)) >> UPPER_20;
		page_dir->dir_entry[i].avl = 0;
		page_dir->dir_entry[i].size = 0; // page size 4KB
		page_dir->dir_entry[i].d = 0;
		page_dir->dir_entry[i].access = 0;
		page_dir->dir_entry[i].cache = 0;
		page_dir->dir_entry[i].write = 1;
		page_dir->dir_entry[i].user = 1;
		page_dir->dir_entry[i].read = 1;
		page_dir->dir_entry[i].pres = 0;	// except table 0 and 1, all other tables are not present
	}

	page_dir->dir_entry[0].frame = ((uint32_t) (page_table_0)) >> PAGING_PT_ADDR;
	page_dir->dir_entry[0].res = ((uint32_t) (page_table_0) >> PAGING_RES)  & PAGING_MASK;
	page_dir->dir_entry[0].pat = ((uint32_t) (page_table_0) >> PAGING_PAT) & PAGING_FINAL_BIT;
	//page tables are present
	page_dir->dir_entry[0].pres = 1;
	//user is 1, supervisor is 0
	page_dir->dir_entry[0].user = 1;

	//page_dir.dir_entry[1].frame = KERNEL_ADDR >> 12;
	//kernel also present
	//page_dir.dir_entry[1].user = 0;
	//page_dir.dir_entry[1].pres = 1;
	//page_dir.dir_entry[1].size = 1;

	page_dir->dir_entry[2].frame = ((uint32_t) (page_table_1)) >> PAGING_PT_ADDR;
	page_dir->dir_entry[2].res = ((uint32_t) (page_table_1) >> PAGING_RES) & PAGING_MASK;
	page_dir->dir_entry[2].pat = ((uint32_t) (page_table_1) >> PAGING_PAT) & PAGING_FINAL_BIT;
	page_dir->dir_entry[2].pres = 1;

	//page_dir.dir_entry[2].frame = ((uint32_t) (page_table_1)) >> 22;
	//page_dir.dir_entry[2].pres = 1;
	//page_dir.dir_entry[2].user = 1;

	//set up the user 128 -132  MB page (4 MB)

	//Don't map this for the kernel directory 
	if (idx != -1)
	{
		// setup the virtual address for the non-kernel directories
		page_dir->dir_entry[INDEX_USER].frame = (uint32_t)(USER_PAGE+idx*KERNEL_ADDR) >> PAGING_PT_ADDR; 
		page_dir->dir_entry[INDEX_USER].pres = 1;
		page_dir->dir_entry[INDEX_USER].size = 1;
		page_dir->dir_entry[INDEX_USER].user = 1;
	}


	page_dir->dir_entry[1].frame = KERNEL_ADDR >> PAGING_PT_ADDR;
	page_dir->dir_entry[1].pres = 1;
	page_dir->dir_entry[1].user = 1;
	page_dir->dir_entry[1].size = 1;

		/* Initialize page table and set flags*/
	for(i = 0; i < NUM_PAGE; i++)
	{
		//page_table_0->table_entry[i].frame = i; //>> UPPER_20;
		page_table_0->table_entry[i].frame = (uint32_t)(i*PAGE_SIZE_4KB) >> PAGING_PAT;
		page_table_0->table_entry[i].avl = 0;
		page_table_0->table_entry[i].global = 1;
		page_table_0->table_entry[i].pat = 0;
		page_table_0->table_entry[i].dirty = 0;
		page_table_0->table_entry[i].access = 0;
		page_table_0->table_entry[i].cache = 0;
		page_table_0->table_entry[i].write = 1;
		page_table_0->table_entry[i].user = 1;
		page_table_0->table_entry[i].read = 1;
		page_table_0->table_entry[i].pres = 1;
	}
	
		for(i = 0; i < NUM_PAGE; i++)
		{
		//printf("%u", i + ((uint32_t) (page_table_1) >> UPPER_20));
		//page_table_1->table_entry[i].frame =  i + ((uint32_t) (page_table_1) >> UPPER_20);
			page_table_1->table_entry[i].frame = (uint32_t)(i*PAGE_SIZE_4KB + 2*KERNEL_ADDR) >> PAGING_PAT;
			page_table_1->table_entry[i].avl = 0;
			page_table_1->table_entry[i].global = 1;
			page_table_1->table_entry[i].pat = 0;
			page_table_1->table_entry[i].dirty = 0;
			page_table_1->table_entry[i].access = 0;
			page_table_1->table_entry[i].cache = 0;
			page_table_1->table_entry[i].write = 1;
			page_table_1->table_entry[i].user = 1;
			page_table_1->table_entry[i].read = 1;
		page_table_1->table_entry[i].pres = 1;
		}
	
	
	
	//int k = 0;
	//for(k = 0; k < 4; k++)
	//	printf("%u\t", page_dir->dir_entry[k].frame);
	//first page table entry should be NULL and have present bit off
	page_table_0->table_entry[0].pres = 0;
	//set cr3 register
	/*
	asm volatile (
		"movl %0, %%cr3":: "r" (page_dir)
	);
	*/
	//set cr4 register
	uint32_t cr4 = 0x0;

	asm volatile(
		"movl %%cr4, %0": "=r"(cr4)
	);

	// turn on Page Size Extensions (enable 4MB and 4kB pages)
	cr4 |= 0x10;
	asm volatile(
		"movl %0, %%cr4":: "r"(cr4)
	);
}



/*Purpose: shift CR3 to the directory we want*/

void cr3_switch(pd_t *pdir1)
{
	asm volatile (
		"movl %0, %%cr3":: "r" (pdir1)
	);
	CURR_PDIR = pdir1;
}
/*
Purpose: enable paging
Arguments: NONE
Called: during boot, before enabling interrupts
Result: enables paging
*/
void enable_paging()
{
	uint32_t cr0;
    asm volatile(
		"movl %%cr0, %0": "=r"(cr0)
	);
    cr0 |= EN_PAGING; // Turn on paging using inline-assembly
    asm volatile(
		"movl %0, %%cr0":: "r"(cr0)
	);
    //printf("I did crapp");
}

/*
uint32_t add_pdir(uint32_t vaddr, int user, int size, pd_t *pdir, int idx)
{
	if(idx >= NUM_TABLE)
	{
		return -1;
	}
	uint32_t v_idx = (vaddr >> 22);
	if(size == 1)
	{
		//4MB page
		pdir->dir_entry[v_idx].frame = (idx * KERNEL_ADDR);
		pdir->dir_entry[v_idx].pres = 1;
		pdir->dir_entry[v_idx].size = 1;
		pdir->dir_entry[v_idx].user = user;
	}

	else
	{
		//4KB page
		pdir.dir_entry[v_idx].frame = ((uint32_t) &(p_tables[idx])) >> 22;
		pdir.dir_entry[v_idx].res = ((uint32_t) &(p_tables[idx]) >> 13)  & 0x1FF;
		pdir.dir_entry[v_idx].pat = ((uint32_t) &(p_tables[idx]) >> 12) & 0x1;
		pdir.dir_entry[v_idx].pres = 1;
		pdir.dir_entry[v_idx].size = 0;
		pdir.dir_entry[v_idx].user = user;
		int i;
		for(i = 0; i < NUM_PAGE; i++)
		{
			p_tables[idx].table_entry[i].frame =  i + ((uint32_t) &(p_tables[idx]) >> UPPER_20);
			p_tables[idx].table_entry[i].avl = 0;
			p_tables[idx].table_entry[i].global = 1;
			p_tables[idx].table_entry[i].pat = 0;
			p_tables[idx].table_entry[i].dirty = 0;
			p_tables[idx].table_entry[i].access = 0;
			p_tables[idx].table_entry[i].cache = 0;
			p_tables[idx].table_entry[i].write = 1;
			p_tables[idx].table_entry[i].user = 1;
			p_tables[idx].table_entry[i].read = 1;
			p_tables[idx].table_entry[i].pres = 1;
		}
	}

	
	idx += 1;
	return 0;
}*/
