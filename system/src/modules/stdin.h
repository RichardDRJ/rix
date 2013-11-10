#ifndef STDIN_H
#define STDIN_H

#include "file.h"

/*struct instream
{
    unsigned char *buffbase;
    unsigned int buffsize;
    unsigned char *end;
    unsigned char *start;
};*/

FILE *getstdinstream(void);

void redirect(FILE *newstream);

FILE *getstdinstream(void);

void setupstdin(void);

void stdinputc(unsigned char);

unsigned char stdingetc(void);

unsigned char stdingets(unsigned char*, unsigned int);

#endif
