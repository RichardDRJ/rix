#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void irqRemap(void);

struct regs
{
    /*  From www.osdever.net. This ties in with the isr common stub.
     *  When an interrupt is called, the registers in the last line
     *  are pushed automatically. Then, the handler for each one 
     *  pushes the interrupt number and the error code. After this,
     *  the common stub does pusha, pushing edi, esi, ..., eax
     *  and then it pushes the segment registers.
     *  After this, it copies esp to eax and pushes eax so that we
     *  then have the pointer to the top of the stack as the first
     *  argument to the handler. This is treated as a pointer to
     *  this struct. */
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int intno, intarg;
    unsigned int eip, cs, eflags, useresp, ss;
};

void irqhandler(struct regs *r);

void faulthandler(struct regs *r);

/* Defines an IDT entry */
struct idtentry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idtptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idtptr idtp;
struct idtentry *idt;

void isrsinstall(void);

void irqsinstall(void);

void createIDTEntry(unsigned short entrynum, unsigned int base, unsigned char access);

void installIDT(void);

void irqInstallHandler(int irq, void (*handler)(struct regs*));

void irqUninstallHandler(int irq);

#endif
