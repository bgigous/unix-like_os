#define MAX_CALL_NUMBER		10
#define PCB_MASK			0xFFFFE000
#define STACK_TEST			0x00700000

# eax - call number
# ebx - first argument
# ecx - second argument (if applicable)
# edx - third argument (if applicable)

# THE STACK

#	old ebx		 <-- esp (kernel's, right?)
#	arbitrary stuff

.globl system_call

system_call:
	
	CMPL	$0, %eax					# assuming call numbers start at 1
	JE		sc_error

	CMPL	$MAX_CALL_NUMBER, %eax		# can't be greater than max call number
	JA		sc_error

	# at this point, eax contains a valid call number

	# Step 1: save register values
	# Step 2: push parameters
	# Step 3: call particular C handler function
	# Step 4: pop off parameters
	# Step 5: restore register values

	# save our register values
	# NOTE: this step is for security purposes
	#		we don't want to leak info to the user
	PUSHL	%edx
	PUSHL	%ecx
	PUSHL	%ebx
	# push arguments (C calling convention)
	# NOTE: Yep, we are pushing the exact same registers again, but this time
	#		it's for the arguments of the system call handler
	PUSHL	%edx						# third arg
	PUSHL	%ecx						# second arg
	PUSHL	%ebx						# first arg
	CALL	*sc_jmp_table(,%eax,4)		# call C function to handle sys call
										# (goto effective address of sc_jmp_table + %eax*4)
	ADDL	$12, %esp					# pop off parameters
	POPL	%ebx						
	POPL	%ecx						# restore registers
	POPL	%edx
sc_return:
	IRET								# return from interrupt (INT 0x80)

sc_error:
	MOVL	$-1, %eax
	JMP		sc_return
										# eax is return value from sys call handler C function

sc_jmp_table:
	.long 0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
