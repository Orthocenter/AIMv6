#ifndef _KERN_EXCEPT_HANDLER_H
#define _KERN_EXCEPT_HANDLER_H

#include <config.h>
#include <asm/except.h>

void change_mode(u32 mode);
void SWI_handler();
void IRQ_handler();

#endif // _KERN_EXCEPT_HANDLER_H

