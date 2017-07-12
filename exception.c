#include "exception.h"
#include "lib.h"
#include "sys_handler.h"
/* Exception handlers */

#define PF_PROT_VIOL		0x00000001	//protection violation
#define PF_WR				0x00000002	//1 - write access caused fault
										//0 - read access caused fault
#define PF_US				0x00000004 	//1 - occured in user mode
										//0 - occured in supervisor mode
#define PF_RSVD				0x00000008	//1 - some reserved bits set to 1
#define PF_ID				0x00000010	//1 - caused by an instruction fetch

#define DIE_BY_EXCEPTION	255
void exc_div0()
{
	cli();
	printf("Division by 0 exception\n");
	int i;
	for ( i = 0; i < 0x0FFFFFFF; i++ );
	printf("Self destruction in 3... ");
	for ( i = 0; i < 0x0FFFFFFF; i++ );
	printf("2... ");
	for ( i = 0; i < 0x0FFFFFFF; i++ );
	printf("1... ");
	for ( i = 0; i < 0x1FFFFFFF; i++ );
	printf("Self destruct exception");
	halt( DIE_BY_EXCEPTION );
	asm volatile(".1: hlt; jmp .1;");
}

void exc_debug()
{
	cli();
	printf("Debug Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_nmi()
{
	cli();
	printf("NMI Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_breakpoint()
{
	cli();
	printf("Breakpoint Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_overflow()
{
	cli();
	printf("Overflow Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_bound_range()
{
	cli();
	printf("Bound Range Exceeded Exception\n"); 
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_opcode()
{
	cli();
	printf("Invalid Opcode Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_dev_not_avail()
{
	cli();
	printf("Device Not Available Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_dbl_fault()
{
	cli();
	printf("Double Fault Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_seg_overrun()
{
	cli();
	printf("Coprocessor segment overrun\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}
void exc_invalid_tss()
{
	cli();
	printf("Invalid TSS Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_seg_not_present()
{
	cli();
	printf("Segment Not Present Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_stack_fault()
{
	cli();
	printf("Stack Fault Exception\n"); 
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_gen_protection()
{
	cli();
	uint32_t err_code;
	asm volatile (
		"POPL	%0": "=r" (err_code)
	);

	printf("General Protection Exception\n");

	asm volatile (
		"popl %0": "=r" (err_code)
	);
	
	//printf("Error code is: 0x%x\n", err_code);

	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_page_fault()
{
	cli();
	printf("Page Fault Exception\n");

	//find page fault linear address
	uint32_t cr2;

	asm volatile (
		"movl %%cr2, %0": "=r" (cr2)
	);

	printf( "Attempt to access linear address 0x%x failed\n", cr2 );

	uint32_t err_code;
	asm volatile(
		"popl %%eax 		;"
		"movl %%eax, %0		;"
		: "=r" (err_code)
	);

	printf("Error code was: %d\n", err_code & 0x1F);

	printf("Caused by:\n");
	if ( err_code & PF_PROT_VIOL )
		printf( "    - protection violation\n");
	else printf("    - non-present page\n");
	if ( err_code & PF_WR )
		printf( "    - write access\n");
	else printf("    - read access\n");
	if ( err_code & PF_US )
		printf( "    - user\n");
	else printf("    - supervisor\n");
	if ( err_code & PF_RSVD )
		printf( "    - reserved bits set to 1\n" );
	if ( err_code & PF_ID )
		printf( "    - instruction fetch\n" );

	

	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_reserved()
{
	cli();
	printf("Got a reserved interrupt. Weird.\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_flt_point()
{
	cli();
	printf("Floating-Point Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_alignemnt()
{
	cli();
	printf("Alignment Check Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

void exc_machine_check()
{
	cli();
	printf("Machine Check Exception\n");
	halt( DIE_BY_EXCEPTION );
	while(1);
}

