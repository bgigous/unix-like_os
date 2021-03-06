#ifndef _ASM
#define _ASM

#include "timer.h"
#include "keyboard.h"
#include "rtc.h"
//#include "lib.h"
//#include "term_driver.h"
#define TIMER_IRQ		0x00
#define KEYBOARD_IRQ	0x01
#define CASCADE_IRQ		0x02
#define	IRQ3			0x03
#define SERIAL_IRQ		0x04
#define IRQ5			0x05
#define IRQ6			0x06
#define IRQ7			0x07
#define RTC_IRQ			0x08
#define IRQ9			0x09
#define IRQ10			0x0A
#define ETH0_IRQ		0x0B
#define MOUSE_IRQ		0x0C
#define IDE0_IRQ		0x0D
#define IRQ14			0x0E


//define interrupt handlers
void default_hndl_asm(void);
void hndl_timer(void);
void hndl_keyboard(void);
void hndl_cascade(void);
void hndl_irq3(void);
void hndl_serial(void);
void hndl_irq5(void);
void hndl_irq6(void);
void hndl_irq7(void);
void hndl_rtc(void);
void hndl_irq9(void);
void hndl_irq10(void);
void hndl_eth0(void);
void hndl_mouse(void);
void hndl_ide0(void);
void hndl_irq14(void);

extern int32_t term_switch;

/* These functions are called by the interrupt handlers. */

/* used for unknown interrupts */
void default_handler()
{
	printf("Got some kind of interrupt\n");
}

/* takes care of timer chip interrupts */
void handler_timer(void){
	//printf("Timer Chip Interrupt\n");
	handle_tick();
	send_eoi(TIMER_IRQ);
}

/* takes care of keyboard events (ie. key presses) */
void handler_keyboard(void){
	handle_press();
	send_eoi(KEYBOARD_IRQ);
}

/* takes care of PIC cascade events */
void handler_cascade(void)
{
	printf("Cascade Interrupt\n");
	send_eoi(CASCADE_IRQ);
}

void handler_irq3(void)
{
	printf("irq3 interrupt\n");
	send_eoi(IRQ3);
}

/* takes care of serial port events */
void handler_serial(void)
{
	printf("serial port interrupt\n");
	send_eoi(SERIAL_IRQ);
}

void handler_irq5(void){
	printf("irq5 interrupt\n");
	send_eoi(IRQ5);
}

void handler_irq6(void){
	printf("irq6 interrupt\n");
	send_eoi(IRQ6);
}

void handler_irq7(void){
	printf("irq7 interrupt\n");
	send_eoi(IRQ7);
}

/* takes care of periodic interrupts from the real time clock */
void handler_rtc(void){
	//printf("RTC INT");
	handle_rtc();
	send_eoi(RTC_IRQ);
}

void handler_irq9(void){
	printf("irq9 interrupt\n");
	send_eoi(IRQ9);
}

void handler_irq10(void){
	printf("irq10 interrupt\n");
	send_eoi(IRQ10);
}

/* takes care of network events */
void handler_eth0(void){
	printf("eth0 interrupt\n");
	send_eoi(ETH0_IRQ);
}

/* takes care of events from the PS/2 mouse */
void handler_mouse(void){
	printf("PS/2 mouse interrupt\n");
	send_eoi(MOUSE_IRQ);
}

/* takes care of events from the hard drive */
void handler_ide0(void){
	printf("ide0 interrupt\n");
	send_eoi(IDE0_IRQ);
}

void handler_irq14(void){
	printf("irq14 interrupt\n");
	send_eoi(IRQ14);
}
#endif

