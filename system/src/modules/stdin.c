#include "stdin.h"
#include "system.h"
#include "walloc.h"
#include "palloc.h"
#include "kprint.h"

static FILE stdinstream;
static FILE *redirectstream = 0;

FILE *getstdinstream(void)
{
    return redirectstream ? redirectstream : &stdinstream;
}

void redirect(FILE *newstream)
{
    redirectstream = newstream;
    ++(redirectstream->currentreadoffset);
}

void removeredirect(void)
{
    redirectstream = 0;
}

void setupstdin(void)
{
    stdinstream.length = 0x00000FFF;
    stdinstream.stream = (unsigned char*)palloc(stdinstream.length);
    stdinstream.currentreadoffset = 0;
    stdinstream.currentwriteoffset = 1;

    memset(stdinstream.stream, 0, stdinstream.length);
}

void stdinputc(unsigned char c)
{
    if(stdinstream.currentreadoffset == stdinstream.currentwriteoffset)
    {
        memset(stdinstream.stream, 0, (unsigned int)(((unsigned char*)(stdinstream.currentwriteoffset + stdinstream.stream)) + 1 - stdinstream.stream));
        stdinstream.currentreadoffset = 0;
        stdinstream.currentwriteoffset = 1;
    }

    if(c == 4 || ((c == '\r' || c == '\n') && *(unsigned char*)(stdinstream.currentwriteoffset + stdinstream.stream) != '\\'))
        ++stdinstream.currentreadoffset;

    if(c == '\b')
        --stdinstream.currentwriteoffset;
    else
        *(unsigned char*)(stdinstream.currentwriteoffset++ + stdinstream.stream) = c;

    writec(c);
}

unsigned char stdingetc(void)
{
    if(stdinstream.currentreadoffset == stdinstream.currentwriteoffset)
    {
        memset(stdinstream.stream, 0, (unsigned int)(((unsigned char*)(stdinstream.currentwriteoffset + stdinstream.stream)) + 1 - stdinstream.stream));
        stdinstream.currentreadoffset = 0;
        stdinstream.currentwriteoffset = 1;
    }

    if(*(unsigned char*)(stdinstream.currentreadoffset + stdinstream.stream) == 4 || stdinstream.currentreadoffset == stdinstream.length - 1)
        return 0;

    while(*((unsigned char*)(stdinstream.currentreadoffset + stdinstream.stream)) == 0);

    return *((unsigned char*)(stdinstream.currentreadoffset++ + stdinstream.stream));
}

unsigned char stdingets(unsigned char* s, unsigned int length)
{
    unsigned char cbuff;
    while(--length > 0 && (cbuff = stdingetc()))
    {
        if(cbuff == 4 || ((cbuff == '\r' || cbuff == '\n') && *(unsigned char*)(stdinstream.currentwriteoffset + stdinstream.stream) != '\\'))
        {
            break;
        }

        *s++ = cbuff;
    }

    *s = 0;

    if(cbuff == 4 || stdinstream.currentreadoffset == stdinstream.length - 1)
        return 0;

    return 1;
}
