#include "system.h"
#include "interrupts.h"
#include "kprint.h"
#include "palloc.h"

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void irq0x80();

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();


extern void idtload();

char *exceptionmessages[] =
{
    "Division by zero",
    "Debug exception",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into detected overflow",
    "Out of bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check exception",
    "Machine check exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};


/*  From http://www.osdever.net/bkerndev/Docs/irqs.htm */
/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 *  is a problem in protected mode, because IDT entry 8 is a
 *  Double Fault! Without remapping, every time IRQ0 fires,
 *  you get a Double Fault Exception, which is NOT actually
 *  what's happening. We send commands to the Programmable
 *  Interrupt Controller (PICs - also called the 8259's) in
 *  order to make IRQ0 to 15 be remapped to IDT entries 32 to
 *  47 */
void irqRemap(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}
/********************************************************/

void *irqhandlers[0x61] =
    { 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0
    };

void irqInstallHandler(int irq, void (*handler)(struct regs*))
{
    irqhandlers[irq] = handler;
}

void irqUninstallHandler(int irq)
{
    irqhandlers[irq] = 0;
}

void irqhandler(struct regs *r)
{
    void (*handler)(struct regs*);

    handler = irqhandlers[r->intno - 0x20];

    if(handler)
    {
        (*handler)(r);
    }

    /* If the IDT entry that was invoked was greater than 40
     *  (meaning IRQ8 - 15), then we need to send an EOI to
     *  the slave controller */
    if (r->intno >= 40)
    {
        outb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
     *  interrupt controller too */
    outb(0x20, 0x20);
}

void isrhandler(struct regs *r)
{
    if(r->intno < 32)
    {
        writes("\n");
        writes(exceptionmessages[r->intno]);
    }

    for(;;);
}

void isrsinstall(void)
{
    createIDTEntry(0, (unsigned int)(&isr0), 0);
    createIDTEntry(1, (unsigned int)(&isr1), 0);
    createIDTEntry(2, (unsigned int)(&isr2), 0);
    createIDTEntry(3, (unsigned int)(&isr3), 0);
    createIDTEntry(4, (unsigned int)(&isr4), 0);
    createIDTEntry(5, (unsigned int)(&isr5), 0);
    createIDTEntry(6, (unsigned int)(&isr6), 0);
    createIDTEntry(7, (unsigned int)(&isr7), 0);
    createIDTEntry(8, (unsigned int)(&isr8), 0);
    createIDTEntry(8, (unsigned int)(&isr9), 0);
    createIDTEntry(10, (unsigned int)(&isr10), 0);
    createIDTEntry(11, (unsigned int)(&isr11), 0);
    createIDTEntry(12, (unsigned int)(&isr12), 0);
    createIDTEntry(13, (unsigned int)(&isr13), 0);
    createIDTEntry(14, (unsigned int)(&isr14), 0);
    createIDTEntry(15, (unsigned int)(&isr15), 0);
    createIDTEntry(16, (unsigned int)(&isr16), 0);
    createIDTEntry(17, (unsigned int)(&isr17), 0);
    createIDTEntry(18, (unsigned int)(&isr18), 0);
    createIDTEntry(19, (unsigned int)(&isr19), 0);
    createIDTEntry(20, (unsigned int)(&isr20), 0);
    createIDTEntry(21, (unsigned int)(&isr21), 0);
    createIDTEntry(22, (unsigned int)(&isr22), 0);
    createIDTEntry(23, (unsigned int)(&isr23), 0);
    createIDTEntry(24, (unsigned int)(&isr24), 0);
    createIDTEntry(25, (unsigned int)(&isr25), 0);
    createIDTEntry(26, (unsigned int)(&isr26), 0);
    createIDTEntry(27, (unsigned int)(&isr27), 0);
    createIDTEntry(28, (unsigned int)(&isr28), 0);
    createIDTEntry(29, (unsigned int)(&isr29), 0);
    createIDTEntry(30, (unsigned int)(&isr30), 0);
    createIDTEntry(31, (unsigned int)(&isr31), 0);
}

void irqsinstall(void)
{
    createIDTEntry(32, (unsigned int)(&irq0), 0);
    createIDTEntry(33, (unsigned int)(&irq1), 0);
    createIDTEntry(34, (unsigned int)(&irq2), 0);
    createIDTEntry(35, (unsigned int)(&irq3), 0);
    createIDTEntry(36, (unsigned int)(&irq4), 0);
    createIDTEntry(37, (unsigned int)(&irq5), 0);
    createIDTEntry(38, (unsigned int)(&irq6), 0);
    createIDTEntry(39, (unsigned int)(&irq7), 0);
    createIDTEntry(40, (unsigned int)(&irq8), 0);
    createIDTEntry(41, (unsigned int)(&irq9), 0);
    createIDTEntry(42, (unsigned int)(&irq10), 0);
    createIDTEntry(43, (unsigned int)(&irq11), 0);
    createIDTEntry(44, (unsigned int)(&irq12), 0);
    createIDTEntry(45, (unsigned int)(&irq13), 0);
    createIDTEntry(46, (unsigned int)(&irq14), 0);
    createIDTEntry(47, (unsigned int)(&irq15), 0);
    createIDTEntry(0x80, (unsigned int)(&irq0x80), 3);
}


void createIDTEntry(unsigned short entrynum, unsigned int base, unsigned char access)
{
    idt[entrynum].sel = 0x08;
    idt[entrynum].base_lo = base & 0xFFFF;
    idt[entrynum].base_hi = base >> 16 & 0xFFFF;
    idt[entrynum].flags = 1;
    idt[entrynum].flags <<= 2;
    idt[entrynum].flags |= access & 3;
    idt[entrynum].flags <<= 5;
    idt[entrynum].flags |= 14;
    idt[entrynum].always0 = 0;
}

void installIDT(void)
{
    idt = (struct idtentry*)palloc(256 * sizeof(struct idtentry));

    irqRemap();

    idtp.limit = sizeof(struct idtentry) * 256 - 1;
    idtp.base = (unsigned int)idt;

    memset (idt, 0, sizeof(struct idtentry) * 256);

    idtload();
}
