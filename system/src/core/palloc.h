#ifndef PALLOC_H
#define PALLOC_H

void setuppalloc(unsigned, unsigned, unsigned char *, unsigned char *);
void *palloc(unsigned int);
void pfree(void *);
void allocpagenum(unsigned short, unsigned);
void freepagenum(unsigned short, unsigned);
long long findfreepage(unsigned*, long long);

#endif
