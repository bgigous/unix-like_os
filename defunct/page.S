#include "page.h"


#define P_ENTRY_MASK 0x006
#define PAGE_BASE_MASK 0xFFFFF000
#define PAGE_BASE_MASK_4MB 0xFFC00000
#define ENTRY_ZERO_MASK 0x007

.globl setup_paging
.type setup_paging @function

setup_paging:
	PUSHL	%ebp
	MOVL	%esp, %ebp			# stack setup

	PUSHL	%eax
	PUSHL	%ebx

	MOVL	%cr4, %ebx
	ORL		$PSE_FLAG, %ebx		# enable PSE (page size extensions) for 4MB pages
	ANDL	$PAE_FLAG, %ebx		# clear PAE (physical address extension) flag
	MOVL	%ebx, %cr4

update_cr3:
	MOVL	$pd, %eax			# eax <- page directory base address
	ANDL	$BASE_ADDR_MASK, %eax	# mask eax to get most significant 20 bits of page directory base address
	MOVL	%cr3, %ebx
	ORL		%eax, %ebx			# update control register 3 with address of our page directory, preserve flags
	MOVL	%ebx, %cr3

init_paging:
	# setup first page directory entry
	MOVL	$pt, %eax				# eax <- first page table base address
	ANDL	$BASE_ADDR_MASK, %eax	# mask eax to get most significant 20 bits of page table base address
	ORL		$PRESENT, %eax			# indicate that this entry is present in physical memory
	MOVL 	$pd, %ebx
	MOVL	%eax, (%ebx)				# insert first pde
	
	# set up second page directory entry
	MOVL	$KERNEL_START, %eax 	# eax <- physical starting address of kernel
	ANDL	$BASE_ADDR_MASK_4MB, %eax	# mask eax to get most significant 10 bits of kernel page base address
	ORL		$PRESENT, %eax
	ORL		$0x2, %eax
	ORL		$0x80, %eax
	MOVL	$pd, %ebx
	MOVL	%eax, 4(%ebx)			# insert second page directory entry


	

	POPL	%ebx
	POPL	%eax
	
	LEAVE
	RET

	
.globl init_table
.type init_table @function
init_table:
	pushl %ebp									#set up stack
	movl %esp, %ebp
	pushl %esi									#counter (1 -> 1023)
	pushl %eax
	pushl %ebx
	xorl %esi, %esi								#counter = 0
	movl $pt, %ebx								#ebx <- pt
	xorl %eax, %eax								#clear eax
	orl $ENTRY_ZERO_MASK, %eax					#present bit off, other flags set
	movl %eax, (%ebx)							#put that into pt[0]
	addl $1, %esi								#counter += 1
	table_loop:
		movl (%ebx, %esi, 4), %eax				#eax <- pt[counter]	
		andl $BASE_ADDR_MASK, %eax				#clear bottom 12
		orl $P_ENTRY_MASK, %eax					#change the present bit and flags
		movl %eax, (%ebx, %esi, 4)				#put it back
		
		leal (, %esi, 4), %eax				#calculate base address
		shl $10, %eax
		andl $BASE_ADDR_MASK, %eax			#get most significant 20 bits
		orl %eax, (%ebx, %esi, 4)				#insert page base address
			
		addl $1, %esi							#counter += 1
		cmpl $1024, %esi						#loop
		jne table_loop
	
	#MOVL	%cr0, %ebx
	#ORL		$PE_FLAG, %ebx		# enable protected mode	
	#MOVL	%ebx, %cr0
	
	#MOVL 	%cr0, %eax							#put cr0 in eax
	#MOVL	$PG_FLAG, %ebx						#set on paging
	#ORL		%ebx, %eax				
	#MOVL	%eax, %cr0	
	
	popl %ebx
	popl %eax
	popl %esi
	
	LEAVE
	RET


	
	
