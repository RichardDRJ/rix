#include "harddiskdriver.h"
#include "system.h"

void readblock(unsigned int blocknum, unsigned char *buffer, unsigned int numblocks, unsigned int drive)
{
    outb(0x1F1, 0x00);
    outb(0x1F2, numblocks);
    outb(0x1F3, (unsigned char)blocknum);
    outb(0x1F4, (unsigned char)(blocknum >> 8));
    outb(0x1F5, (unsigned char)(blocknum >> 16));
    outb(0x1F6, 0xE0 | (drive << 4) | ((blocknum >> 24) & 0x0F));
    outb(0x1F7, 0x20);

    while (!(inb(0x1F7) & 0x08)) {}

    unsigned short wordIndex;

    for (wordIndex = 0; wordIndex < 256; ++wordIndex)
    {
        unsigned short tmpword = inw(0x1F0);
        buffer[wordIndex * 2] = (unsigned char)tmpword;
        buffer[wordIndex * 2 + 1] = (unsigned char)(tmpword >> 8);
    }
}
