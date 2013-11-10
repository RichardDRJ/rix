#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "interrupts.h"
#include "systemcallcodes.h"

void initSystemCalls(void);
void systemCall(struct regs *r);

#endif
