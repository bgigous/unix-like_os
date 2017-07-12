/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 * group04
 * spring 2014
 * ece 391
 */

#include "i8259.h"
#include "lib.h"

#define DAT_MASK 0xFF
#define IRQ_MAX	 15

/* Interrupt masks to determine which interrupts
are enabled and disabled */
uint8_t master_mask;  		// for IRQs 0-7 
uint8_t slave_mask; 		// for IRQs 8-15

void
i8259_init(void)
{
	/* Initialize the PIC function
	 * NOTE: We will assume that interrupts have 
	 * 		 been masked already with CLI before
	 * 		 this function is called.
	 */

	/* mask all interrupts by default */
	master_mask  = DAT_MASK;
	slave_mask   = DAT_MASK;

	/* send PICs initialization code */
	outb( ICW1,		MASTER_8259_PORT);
	outb( ICW1,		SLAVE_8259_PORT);
	
	/* send PICs location of vectors */
	outb( ICW2_MASTER,	MASTER_8259_PORT2);
	outb( ICW2_SLAVE,  	SLAVE_8259_PORT2 );
	
	/* Setup inter PIC communications */
	outb( ICW3_MASTER, 	MASTER_8259_PORT2 );
	outb( ICW3_SLAVE,  	SLAVE_8259_PORT2);
	
	/* set the PICs modes */
	outb( ICW4,		MASTER_8259_PORT2);
	outb( ICW4,		SLAVE_8259_PORT2);
	
	/* initially mask all interrupts */
	outb( master_mask,	MASTER_8259_PORT2 );
	outb( slave_mask, 	SLAVE_8259_PORT2 );
}

void
enable_irq(uint32_t irq_num)
{
	/* Enable (unmask) the specified IRQ */
	if(irq_num <= 7)	// select MASTER SLAVE PIC
	{
		/* interrupt is for the MASTER PIC */
		master_mask &= ~(1 << irq_num);
		outb( master_mask,	MASTER_8259_PORT2 );
		return;
	}
	else if (irq_num <= 15)
	{
		/* SLAVE PIC 
		 * but first signal MASTER PIC on SLAVE_IRQ port*/	
		master_mask &= ~(1 << SLAVE_IRQ);
		outb( master_mask,	MASTER_8259_PORT2 );
		
		/* second signal SLAVE PIC */
		irq_num -= 8;
		slave_mask &= ~(1 << irq_num);
		outb( slave_mask, 		SLAVE_8259_PORT2 );
		return;
	}
}

void
disable_irq(uint32_t irq_num)
{
	/* Disable (mask) the specified IRQ */
	if(irq_num <= 7)	// select MASTER SLAVE PIC
	{
		/* MASTER PIC */
		master_mask |= (1 << irq_num);
		outb( master_mask, MASTER_8259_PORT2 );
	}
	else if (irq_num <= IRQ_MAX)
	{
		/* SLAVE PIC */
		master_mask |= (1 << SLAVE_IRQ);
		outb( master_mask,	MASTER_8259_PORT2 );
		
		irq_num -= 8;					// adjust for SLAVE
		slave_mask |= (1 << irq_num);	// update the mask
		outb( slave_mask, 		SLAVE_8259_PORT2 );
	}
}

void
send_eoi(uint32_t irq_num)
{
	/* Send end-of-interrupt signal for the specified IRQ */
	if ( irq_num >= 8 )			// select MASTER SLAVE PIC
	{
		/* SLAVE PIC */
		outb( EOI | (irq_num-8),	SLAVE_8259_PORT );	// SLAVE EOI
		outb( EOI | SLAVE_IRQ,		MASTER_8259_PORT );	// MASTER EOI
	}
	else
	{
		/* MASTER */
		outb( EOI | irq_num, 		MASTER_8259_PORT );	// MASTER EOI
	}
}

