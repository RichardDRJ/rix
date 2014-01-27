#include "system.h"

/*inline void outb(unsigned short port, unsigned char val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile( "inb %1, %0" : "=a"(ret) : "nd"(port) );
    return ret;
}

inline unsigned short inw(unsigned short port)
{
    unsigned short ret;
    asm volatile( "inw %1, %0" : "=a"(ret) : "nd"(port) );
    return ret;
}*/

void memset(void *ptr, int ivalue, unsigned int size)
{
    unsigned char val = (unsigned char)ivalue;
    unsigned int iPtrEnd = (unsigned int)ptr;
    iPtrEnd += size;

    void *ptrEnd = (void*)iPtrEnd;

    while(ptr < ptrEnd)
    {
        *(unsigned char*)ptr = val;
        ++ptr;
    }
}

void memcpy(void *dest, const void *src, unsigned int size)
{
    char *cDest = (char*)dest;
    const char *cSrc = (const char*)src;
    while(size--)
    {
        *cDest++ = *cSrc++;
    }
}
