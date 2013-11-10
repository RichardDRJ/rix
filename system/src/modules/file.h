#ifndef FILE_H
#define FILE_H

typedef struct file
{
    unsigned int length;
    unsigned char *stream;
    unsigned int currentreadoffset;
    unsigned int currentwriteoffset;
} FILE;

#endif
