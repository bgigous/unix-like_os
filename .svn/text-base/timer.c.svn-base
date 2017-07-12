/*NOTES
* the pit channel 2 gate is controlled by io port 0x61 bit 0
*channel 0 - connected to irq0
*channel 1 - unusable
*channel 2 - pc speaker but can be used for other purposes
*we need to decide how we are going to program the poit usign port 0x61
* if a frequency of 100hz is desired the divisor needed is 11913182/100 = 11931- split this value into a high and low byte
*/

#include "timer.h"

volatile int pit_read_flag = 0;
int timer_ticks = 0;
void init_timer()
{
	unsigned long flags;
	cli_and_save(flags);

	enable_irq(TIMER_IRQ);
	/*asm volatile (
		"movl $0x10000, %%eax;"
		"cmpl $18, %%ebx;")*/
	restore_flags(flags);
}

void handle_tick(void)
{
	//"r" (irq0_frequency), "r" (irq0_ms), "r" (system_timer_fractions), "r" (system_timer_ms)
	//int system_timer_fractions = 0;
	//int system_timer_ms = 0;
	//int irq0_fractions  = 0;
	//int irq0_ms = 0;
	//int irq0_frequency = 0;
	//int pit_reload_value = 0;
	timer_ticks++;
	//if (timer_ticks % 18 == 0)
	//printf("tick %d\n",timer_ticks );
	pit_read_flag = 0;
	/*asm volatile(
		"leave 	;"
		"iret 	;"
		:		
		:
		:"memory");
	*/
	/*asm volatile(
		"PUSHL %%eax	;"
		"PUSHL %%ebx	;"
		"MOVL $0, %%eax		;"
		"MOVL $0, %%ebx		;"
		"ADDL $0, %%eax		;"
		"ADDL $0, %%ebx		;"
		"MOVB $0x20, %%al;"
		"OUTB %%al, $0x20;"
		"POPL %%ebx;"
		"POPL %%eax;"
		"IRET;"
		:
		:
		:"%eax" , "%ebx"
		);*/
	

}
int32_t timer_set(int hz)
{
	if (hz < 0)
	{
		return -1 ;
	}

	int divisor = MAX_FREQ_PIT / hz;
	outb(0x36, PIT_CMDREG);
	outb(divisor & 0xFF, PIT_CHANNEL0);
	outb(divisor>>8, PIT_CHANNEL0);
	return 0;

}

void speaker_set(int hz)
{
	int divisor = MAX_FREQ_PIT/ hz;
	outb(0xb6, PIT_CMDREG);
	outb((unsigned char) (divisor), PIT_CHANNEL2);
	outb((unsigned char) (divisor>>8), PIT_CHANNEL2);

}

int32_t pit_read()
{
	unsigned long flags;
	cli_and_save(flags);
	pit_read_flag = 1;
	restore_flags(flags);
	while (pit_read_flag != 0 ) {}//keep spinning
	return 0;
}
