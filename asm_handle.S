# make all handlers global
.globl default_hndl_asm
.globl hndl_timer
.globl hndl_keyboard
.globl hndl_cascade
.globl hndl_irq3
.globl hndl_serial
.globl hndl_irq5
.globl hndl_irq6
.globl hndl_irq7
.globl hndl_rtc
.globl hndl_irq9
.globl hndl_irq10
.globl hndl_eth0
.globl hndl_mouse
.globl hndl_ide0
.globl hndl_irq14

.globl term_switch

# these get called when we receive interrupts
# they call another function to handle the respective interrupt

default_hndl_asm:
	pushal
	call default_handler
	popal
	iret
	
hndl_timer:
	pushal
	call handler_timer
	popal
	iret
	
hndl_keyboard:
	pushal
	# keep track of esp for terminal switching
	# we can clobber eax since it's been pushed
	movl	CURR_TASK, %ebx
	movl	%esp, 4(%ebx)		// CURR_TASK->ESP = %esp
	movl	%ebp, 8(%ebx)
	call handler_keyboard
#	movl	CURR_TASK, %ebx
#	movl	4(%ebx), %esp
#	movl	8(%ebx), %ebp
	popal	
	iret
	
hndl_cascade:
	pushal
	call handler_cascade
	popal
	iret
	
hndl_irq3:
	pushal
	call handler_irq3
	popal
	iret
	
hndl_serial:
	pushal
	call handler_serial
	popal
	iret

hndl_irq5:
	pushal
	call handler_irq5
	popal
	iret

hndl_irq6:
	pushal
	call handler_irq6
	popal
	iret

hndl_irq7:
	pushal
	call handler_irq7
	popal
	iret

hndl_rtc:
	pushal
	call handler_rtc
	popal
	iret

hndl_irq9:
	pushal
	call handler_irq9
	popal
	iret

hndl_irq10:
	pushal
	call handler_irq10
	popal
	iret

hndl_eth0:
	pushal
	call handler_eth0
	popal
	iret

hndl_mouse:
	pushal
	call handler_mouse
	popal
	iret

hndl_ide0:
	pushal
	call handler_ide0
	popal
	iret

hndl_irq14:
	pushal
	call handler_irq14
	popal
	iret

