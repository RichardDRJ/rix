#ifndef WALLOC_H
#define WALLOC_H

typedef struct walloctable
{
    unsigned int nextwalloc;
    unsigned int endofwallocablemem;
} walloctable_t;

void *walloc(unsigned int size);

void setupwalloc(unsigned int startofmem, unsigned int endofmem);

#endif
