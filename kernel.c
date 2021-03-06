/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "keyboard.h"
#include "rtc.h"
#include "keyboard.h"
#include "asm_handle.h"
#include "paging.h"
#include "exception.h"
#include "sys_call.h"
#include "filesys.h"
#include "term_driver.h"
#include "timer.h"
//#include "pcb.h"

// temp included

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

#define NUM_INT 15
#define NUM_EXC 19

#define TEST_RTC 0
/* NOTE: filesys is initialized regardless of if this is 1 or not */
#define TEST_FILESYS 0
#define TEST_TERM 0

#define USER_PAGE_KERNEL_TEST 0x8000000

#define SYS_CALL_VECTOR			0x80
//default handler for sys_call, etc.

//function pointer type
typedef void (*Handler)(void);

//jumptable of exceptions
Handler jump_exceptions[19] =	{
								exc_div0, exc_debug, exc_nmi, exc_breakpoint,
								exc_overflow, exc_bound_range, exc_opcode,
								exc_dev_not_avail, exc_dbl_fault, exc_seg_overrun,
								exc_invalid_tss, exc_seg_not_present, exc_stack_fault, 
								exc_gen_protection, exc_page_fault, exc_reserved, 
								exc_flt_point, exc_alignemnt, exc_machine_check
								};

//handler functions for interrupts (IRQ)
//jumptable of interrupt handlers
Handler jump_int[15] = 		{
							hndl_timer, hndl_keyboard, hndl_cascade, hndl_irq3,
							hndl_serial, hndl_irq5, hndl_irq6, hndl_irq7,
							hndl_rtc, hndl_irq9, hndl_irq10, hndl_eth0,
							hndl_mouse, hndl_ide0, hndl_irq14
							};

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	int i;
	uint32_t * filesystem_ptr;
	
	multiboot_info_t *mbi;
	
	//enable_paging();
	/* Clear the screen. */
	clear();

	printf( "0x%x\n", MULTIBOOT_BOOTLOADER_MAGIC );
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	module_t* mod;
	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i, j;
		//edited
		mod = (module_t*)mbi->mods_addr;
		filesystem_ptr = (uint32_t*)mod->mod_start;
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<4; i++)
			{
				printf("0x");
				for ( j = 3; j >= 0; j-- )
				{
					printf("%x", *((char*)(mod->mod_start+(i*4)+j)));
				}
				printf(" ");
			}
			printf("\n");
			mod_count++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

/*
	printf("address: 0x%x\n", mbi->mods_addr );

	printf("value 0x");
	for( i = 3; i >= 0; i-- )
	{
		prinf("%x", (unsigned char)());
	}
	printf("\n");


*/

	//set in default values (we don't cover some of the vectors otherwise)
	for(i = 0; i < NUM_VEC; i++)
	{
		/* the reserved bits tell the CPU that these are interrupt gates */
		idt_desc_t idt_entry;
		idt_entry.seg_selector 	= KERNEL_CS;
		SET_IDT_ENTRY(idt_entry, default_hndl_asm);
		idt_entry.reserved4		= 0;
		idt_entry.reserved3		= 1;
		idt_entry.reserved2		= 1;
		idt_entry.reserved1		= 1;
		idt_entry.size			= 0;
		idt_entry.reserved0		= 0;
		idt_entry.dpl			= 0;
		idt_entry.present		= 1;
		idt[i] = idt_entry;
	}

	//replace handler for our defined exceptions
	for(i = 0; i < NUM_EXC; i++)
	{
		idt_desc_t idt_entry;
		SET_IDT_ENTRY(idt_entry, jump_exceptions[i]);
		idt_entry.seg_selector 	= KERNEL_CS;
		idt_entry.reserved4		= 0;
		idt_entry.reserved3		= 1;
		idt_entry.reserved2		= 1;
		idt_entry.reserved1		= 1;
		idt_entry.size			= 1;
		idt_entry.reserved0		= 0;
		idt_entry.dpl			= 0;
		idt_entry.present		= 1;
		idt[i] = idt_entry;
	}
	
	//replace handler for our interrupt handlers
	
	for(i = 0; i < NUM_INT; i++)
	{
		idt_desc_t idt_entry;
		SET_IDT_ENTRY(idt_entry, jump_int[i]);
		idt_entry.seg_selector 	= KERNEL_CS;
		idt_entry.reserved4		= 0;
		idt_entry.reserved3		= 1;
		idt_entry.reserved2		= 1;
		idt_entry.reserved1		= 1;
		idt_entry.size			= 1;
		idt_entry.reserved0		= 0;
		idt_entry.dpl			= 0;
		idt_entry.present		= 1;
		//starts at 32 in the table
		idt[i+32] = idt_entry;
	}

	// make an entry for system calls
	{
		idt_desc_t idt_entry;
		SET_IDT_ENTRY(idt_entry, system_call);
		idt_entry.seg_selector 	= KERNEL_CS;
		idt_entry.reserved4		= 0;
		idt_entry.reserved3		= 1;
		idt_entry.reserved2		= 1;
		idt_entry.reserved1		= 1;
		idt_entry.size			= 1; // wtf is this? Intel manual says nothing
		idt_entry.reserved0		= 0;
		idt_entry.dpl			= 3; // allow user to make system calls
		idt_entry.present		= 1;
		//starts at 32 in the table
		idt[SYS_CALL_VECTOR] = idt_entry;		// 0x80 is for system calls
	}
	
	//load the idt pointer
	lidt(idt_desc_ptr);
	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}
	

	/* Stop interrupts and save flags */
	//cli_and_save( flags );

	/* Init the PIC */
	i8259_init();

	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */

	/* Enable keyboard interrupt requests */
	//keyboard_init();
	/* Enable real time clock interrupt requests */
	//rtc_init();

	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simply close
	 * without showing you any output */
	sti();
	
	/* Setup and enable paging */
	init_paging(&p_table_0_kernel, &p_table_1_kernel, &kernel_directory, -1);
	cr3_switch(&kernel_directory);
	enable_paging();
	//last arg of init_paging is 
	init_paging(&p_table_0[0], &p_table_1[0], &pdir[0], 0);
	init_paging(&p_table_0[1], &p_table_1[1], &pdir[1], 1);
	init_paging(&p_table_0[2], &p_table_1[2], &pdir[2], 2);
	init_paging(&p_table_0[3], &p_table_1[3], &pdir[3], 3);
	init_paging(&p_table_0[4], &p_table_1[4], &pdir[4], 4);
	init_paging(&p_table_0[5], &p_table_1[5], &pdir[5], 5);
	
	/* we might as well just initialize the filesys without having to test it */
	init_filesys( filesystem_ptr );
	term_open((const void *)"/dev/terminal");
//	rtc_open((const void *)"/dev/terminal");
	//init_timer();

//	for ( i = 0; i < MAX_PROC; i++ )
//	{
//		int32_t * pcb_pid = (int32_t*)(KERNEL_END - (pid+1) * PAGE_SIZE_4KB*2);
//	}

	dentry_t dentry;
	// if any of these fail...
	if ( read_dentry_by_name((uint8_t*)"shell", &dentry) )
		printf( "you melted up when you tried to lode shell in 3 places\n"); // then this
	if ( ( loder( dentry.inode_idx ) == -1 || loder( dentry.inode_idx ) == -1 ) )
	{
		printf( "you melted up when you tried to lode shell in 3 places\n"); // then this
	}
	//terminal_switch(0, 0);
	//set_term_color(0);
	/* Execute the first program (`shell') ... */
	//uint8_t * program = (uint8_t*)"shell";
	//while(k < 3)
	//{
	//	asm volatile(	"MOVL	$2, %%eax		;"
	//					"MOVL	%0, %%ebx		;"
	//					"INT	$0x80			;"
//						:
//						: "r" (program)					
//						: "%eax", "%ebx"
	//	);
	//	k++;
	//}
	execute((uint8_t*)"shell");
	//asm volatile( "kernel_return:" );
	
	//printf("back in kernel! YAY!\n");
	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}

