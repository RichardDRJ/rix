/*  Walloc - watermark memory allocation. Forget about freeing!
 *  All we need is mindless consumption! Just think of it like
 *  feeling sick because you ate too much too fast, but instead
 *  of food it's memory and instead of it being because you ate
 *  too fast it's because I'm running out of time! Also because
 *  I'm mildly confused by memory management and it's the sort
 *  of thing I would spend weeks on. */

#include "walloc.h"
#include "system.h"

static walloctable_t *wTable;

void setupwalloc(unsigned int startofmem, unsigned int endofmem)
{
    wTable = (walloctable_t*)startofmem;
    wTable->nextwalloc = startofmem + sizeof(walloctable_t);
    wTable->endofwallocablemem = endofmem;
}

void *walloc(unsigned int size)
{
    if(wTable->nextwalloc + size >= wTable->endofwallocablemem)
        return 0;
    void *ret = (void*)wTable->nextwalloc;
    wTable->nextwalloc += size;
    return ret;
}
