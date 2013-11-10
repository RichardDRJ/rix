#include "kprint.h"
#include "system.h"

static unsigned short currentLine;
static unsigned short currentColumn;
static unsigned short attrib = 0x000F;

/* From http://wiki.osdev.org/Text_Mode_Cursor */
void updatecursor(void)
{
    unsigned short position=(currentLine * CHARSPERLINE) + currentColumn;
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

void writec(char c)
{
    switch(c)
    {
        case '\r':
        case '\n':
            newLine();
            return;
        case '\t':
            incChar(4);
            return;
        case '\b':
            backspace();
            return;
    }

    volatile unsigned short *where;
    where = (unsigned short*)(VIDMEMSTART + (currentColumn + (currentLine * CHARSPERLINE)) * 2);
    *where = c | (attrib << 8);
    incChar(1);
}

void setfc(unsigned char fc)
{
    attrib &= 0xF0;
    attrib |= fc;
}

void setbc(unsigned char bc)
{
    attrib &= 0x0F;
    attrib |= (bc << 4);
}

void backspace(void)
{
    
    if(currentColumn > 0)
        currentColumn -= 1;
    else if(currentLine > 0)
    {
        --currentLine;
        currentColumn = CHARSPERLINE - 1;
    }
    volatile unsigned short *where;
    where = (unsigned short*)(VIDMEMSTART + (currentColumn + (currentLine * CHARSPERLINE)) * 2);
    *where = 0 | (attrib << 8);
}

void incChar(unsigned short cols)
{
    currentColumn += cols;
    while(currentColumn >= CHARSPERLINE)
    {
        currentColumn %= CHARSPERLINE;
        ++currentLine;

        if(currentLine >= LINESPERSCREEN)
        {
            memcpy((void*)(VIDMEMSTART), (void*)(VIDMEMSTART + 2 * CHARSPERLINE), (LINESPERSCREEN - 1) * 160);
            memset((void*)(VIDMEMSTART + ((LINESPERSCREEN - 1) * CHARSPERLINE) * 2), 0, CHARSPERLINE * 2);
            currentLine = LINESPERSCREEN - 1;
        }
    }

    updatecursor();
}

void cls(void)
{
    memset((void*)(VIDMEMSTART), 0, (LINESPERSCREEN - 1) * 160);
    updatecursor();
    writes("enigmOS :> ");
}

void newLine(void)
{
    currentColumn = 0;
    ++currentLine;

    if(currentLine >= LINESPERSCREEN)
    {
        memcpy((void*)(VIDMEMSTART), (void*)(VIDMEMSTART + 2 * CHARSPERLINE), (LINESPERSCREEN - 1) * 160);
        memset((void*)(VIDMEMSTART + ((LINESPERSCREEN - 1) * CHARSPERLINE) * 2), 0, CHARSPERLINE * 2);
        currentLine = LINESPERSCREEN - 1;
    }

    updatecursor();

    writes("enigmOS :> ");
}

void writes(char *message)
{
    while(*message)
    {
        writec(*(message++));
    }
}
