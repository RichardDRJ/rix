#include "keyboarddriver.h"
#include "system.h"
#include "stdin.h"
#include "kprint.h"


/*  From http://www.osdever.net/bkerndev/Docs/keyboard.htm. */

unsigned char kbd[128] = {
0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r','t',
'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
'`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

unsigned char kbdshift[128] = {
0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R','T',
'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',
'`', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

unsigned char kbdcaps[128] = {
00, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R','T',
'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',
'`', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

static unsigned char controls;
// capslock, control, shift, alt

void handleKeyPress()
{
    unsigned char inc = inb(0x60);

    if(!(inc & 80))
    {
        switch(inc)
        {
            case CAPSLOCK:
                controls ^= 1;
                break;
            case LSHIFT:
            case RSHIFT:
                controls |= 2;
                break;
        }   
    }
    else
    {
        switch(inc)
        {
            case LSHIFT:
            case RSHIFT:
                controls &= ~2;
                break;
        }
        
    }

    unsigned char c = ((controls & 1) ? kbdcaps[inc] : ((controls & 2) ? kbdshift[inc] : kbd[inc]));

    if(!(inc & 0x80) && c)
    {
        stdinputc(c);
    }
}
