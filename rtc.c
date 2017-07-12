#include "i8259.h"
#include "rtc.h"

#define RTC_IRQ			0x08
#define MAX_FREQ 		32768
#define DEFAULT_FREQUENCY 	1024
#define INITIAL_FREQUENCY 	2 
#define CLEAR_UPPER 		0x0F
volatile int32_t rtc_read_flag = 0;
//static int32_t current_frequency = 2; 
void rtc_init(void)
{
	unsigned long flags;
	cli_and_save(flags);
	outb(0x8B, RTC_OUT);		// select register B, and disable NMI
	char prev=inb(RTC_IN);		// read the current value of register B
	outb(0x8B, RTC_OUT);		// set the inex again (a read will reset the index to register D)
	outb(prev | 0x40, RTC_IN);
	enable_irq(RTC_IRQ);
	outb(0x8A, RTC_OUT);		// set index to register A, disable NMI
	prev=inb(RTC_IN);			// get initial value of register A
	outb(0x8A, RTC_OUT);		// reset index to A
	outb((prev & 0xF0) | 0x0D, RTC_IN); 
	restore_flags(flags);
	
}
/*
INPUT:
OUTPUT:
SIDE EFFECTS: handles the RTC
*/
void handle_rtc(void)
{
	/* according to lib.h the format for 
	outb(data, port) I switched next statement
	around TR */
	sti();
	unsigned long flags;
	cli_and_save(flags);
	outb(0x0C, RTC_OUT);		// select register C
	inb(RTC_IN);
	rtc_read_flag = 0;
	restore_flags(flags);
}

/*
INPUT:
OUTPUT:0 
SIDE EFFECTS: changes the frequency to 2 HZ
*/
int32_t rtc_open(const uint8_t *filename)
{
	rtc_init();
	//printf("I am in rtc_open \n");
	unsigned long arg=2;
	
	//use CMOS_READ and CMOS_WRITE to set the actual frequency
	//set defualt frequency to 2 Hz
	set_freq(arg);//
	return 0;


}
/*
INPUT: frequency to be set : initially to 2Hz
OUTPUT: 
SIDE EFFECTS:periodic ints set to a specified frequency
*/
int32_t set_freq(unsigned long arg)
{
	int temp=0;
	int count=0;
	unsigned long flags;
	unsigned char val;
	if ((arg<INITIAL_FREQUENCY) || (arg>DEFAULT_FREQUENCY)) //we are supposed to restrict user set freq to [2,1024]
	{	
		return -1;//or just return -1 maybe ??
}
	//check if the frequency is a multiple of 2
	if (arg % 2 !=0 )
		return -1; //since freq is not a multiple of 2


	//to find the actual rate in range 2-15
	temp = MAX_FREQ / arg ;
	while (temp>=1)
	{
		temp>>=1;
		count+=1;
	}
	
	val = count;
	//printf("The value is %d", val);
	//get bits 0-4 the formula for freq is 32768>> (val-1)
	val &= CLEAR_UPPER;	
	cli_and_save(flags);
	outb(0x0A,RTC_OUT);
	char prev=inb(RTC_IN);
	outb(0x0A,RTC_OUT);
	outb(((prev & 0xF0) | val),RTC_IN);
	
	restore_flags(flags);
	return 0;
} 
/*
INPUT: accept a 4 byte integer (int32_t)
OUTPUT: 0 
SIDEEFFECTS: write a frequency to the rtc
*/
int32_t rtc_write(int32_t fd,const void *buf, int32_t nbytes)
{
	int32_t frequency; //frequency to set it to 2
	if (nbytes != 4)
		return -1;
	frequency= *((int32_t *)buf);
	if (frequency == 0) 
		return -1;
	if (frequency % 2 !=0 )
		return -1;
	if (frequency > DEFAULT_FREQUENCY)
	{
		//printf("skipping this frequency %d\n",frequency);
		return -1;
		}
	set_freq(frequency);
	return 0;
}
/*
INPUT:
OUTPUT:0 
SIDE EFFECTS: sets the RTC to the default frequency of 1024 HZ
*/
int32_t rtc_close(int32_t fd)
{
	//Change the no of users at a frequency level 
	unsigned long default_freq= DEFAULT_FREQUENCY;
	//default is 1024 HZ - referring osdev.

  
	set_freq(default_freq);
	return 0;
}
/*
INPUT:
OUTPUT: 0 (when another interrupt has occured)
SIDE EFFECTS: wait until the next interrupt
*/
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes)
{
	//set a volatile flag-> wait for int handler to clear it -> then return 0;
	unsigned long flags;
	cli_and_save(flags);
	rtc_read_flag = 1;
	restore_flags(flags);
	while (rtc_read_flag != 0 ) {}//keep spinning
	return 0;
}

