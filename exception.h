#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "sys_handler.h"
/* Declarations of exception handlers */

void exc_div0(void);
void exc_debug(void);
void exc_nmi(void);
void exc_breakpoint(void);
void exc_overflow(void);
void exc_bound_range(void);
void exc_opcode(void);
void exc_dev_not_avail(void);
void exc_dbl_fault(void);
void exc_seg_overrun(void);
void exc_invalid_tss(void);
void exc_seg_not_present(void);
void exc_stack_fault(void);
void exc_gen_protection(void);
void exc_page_fault(void);
void exc_reserved(void);
void exc_flt_point(void);
void exc_alignemnt(void);
void exc_machine_check(void);

#endif

