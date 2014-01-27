#ifndef SYSTEM_H
#define SYSTEM_H

#define BREAK __asm("xchg %bx, %bx")

static inline void outb(unsigned short port, unsigned char val)
{
    __asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    __asm volatile( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline unsigned short inw(unsigned short port)
{
    unsigned short ret;
    __asm volatile( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

/*inline void outb(unsigned short, unsigned char);
inline unsigned char inb(unsigned short);
inline unsigned short inw(unsigned short);*/

void memset(void*, int, unsigned int);
void memcpy(void*, const void*, unsigned int);

#endif
