#include "palloc.h"
#include "system.h"

static long long map4klength;
static unsigned *map4k;
static long long map8klength;
static unsigned *map8k;
static long long map16klength;
static unsigned *map16k;
static long long map32klength;
static unsigned *map32k;
static long long map64klength;
static unsigned *map64k;

/*  Mark the used ones as used. */
void setuppalloc(unsigned kernelstart, unsigned kernelend, __attribute__((__unused__))unsigned char *ptrmemtablestart, unsigned char *ptrmemtableend)
{
    while(*(unsigned*)(ptrmemtableend - 8) != 1)
        ptrmemtableend -= 24;

    unsigned long long memsize = *(unsigned long long*)(ptrmemtableend - 16) + *(unsigned long long*)(ptrmemtableend - 24);
    map4klength = memsize / (0x1000 * 32);      /* Each entry in the array can represent 32 different
                                                   pages, each 4kB long. */
    map8klength = map4klength / 2;
    map16klength = map8klength / 2;
    map32klength = map16klength / 2;
    map64klength = map32klength / 2;


    /*  Create page maps and zero them. */
    map4k = (unsigned*)0x100000;
    map8k = map4k + map4klength;
    map16k = map8k + map8klength;
    map32k = map16k + map16klength;
    map64k = map32k + map32klength;

    memset(map4k, 0, (map4klength + map8klength + map16klength + map32klength + map64klength) * sizeof(unsigned));

    unsigned lowmempagestart = 0;
    unsigned lowmempageend = 0x100000 / 0x1000;
    for(unsigned i = lowmempagestart; i <= lowmempageend; ++i)
        allocpagenum(4, i);

    unsigned kernelpagestart = kernelstart / 0x1000;
    unsigned kernelpageend = kernelend / 0x1000;

    for(unsigned i = kernelpagestart; i <= kernelpageend; ++i)
        allocpagenum(4, i);

    unsigned pallocmappagestart = (unsigned)map4k / 0x1000;
    unsigned pallocmappageend = ((unsigned)map64k + map64klength) / 0x1000;

    for(unsigned i = pallocmappagestart; i <= pallocmappageend; ++i)
        allocpagenum(4, i);
}

void allocpagenum(unsigned short mapsizek, unsigned pagenum)
{
    unsigned numpages = 1;

    unsigned initialpagenum = pagenum;
    unsigned initialmapsizek = mapsizek;
    unsigned initialnumpages = numpages;

    unsigned pagearrayindex;
    unsigned pagearrayoffset;

    unsigned char direction = 3;

    while(direction & 1)
    {
        unsigned pindex = numpages;
        while(pindex-- > 0)
        {
            switch(mapsizek)
            {
                case 4:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map4k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 8:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map8k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 16:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map16k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 32:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map32k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 64:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map64k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;
                default:
                    direction &= ~1;
                    break;
            }
            ++pagenum;
        }

        mapsizek /= 2;
        pagenum *= 2;
        numpages *= 2;
    }

    mapsizek = initialmapsizek;
    pagenum = initialpagenum;
    numpages = initialnumpages;

    while(direction & 2)
    {
        mapsizek *= 2;
        pagenum /= 2;
        numpages += 1;
        numpages /= 2;

        unsigned pindex = numpages;
        while(pindex-- > 0)
        {
            switch(mapsizek)
            {

                case 4:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map4k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 8:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map8k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 16:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map16k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 32:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map32k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;

                case 64:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map64k[pagearrayindex] |= 1 << (31 - pagearrayoffset);
                    break;
                default:
                    direction &= ~2;
                    break;
            }
            ++pagenum;
        }
    }
}

void freepagenum(unsigned short mapsizek, unsigned pagenum)
{
    unsigned numpages = 1;

    unsigned initialpagenum = pagenum;
    unsigned initialmapsizek = mapsizek;
    unsigned initialnumpages = numpages;

    unsigned pagearrayindex;
    unsigned pagearrayoffset;

    unsigned char direction = 3;

    while(direction & 1)
    {
        unsigned pindex = numpages;
        while(pindex-- > 0)
        {

            switch(mapsizek)
            {

                case 4:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map4k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 8:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map8k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 16:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map16k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 32:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map32k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 64:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map64k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;
                default:
                    direction &= ~1;
                    break;
            }
            ++pagenum;
        }
        mapsizek /= 2;
        pagenum *= 2;
        numpages *= 2;
    }

    mapsizek = initialmapsizek;
    pagenum = initialpagenum;
    numpages = initialnumpages;

    while(direction & 2)
    {
        mapsizek *= 2;
        pagenum /= 2;
        numpages += 1;
        numpages /= 2;
        unsigned pindex = numpages;
        while(pindex-- > 0)
        {

            switch(mapsizek)
            {

                case 4:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map4k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 8:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map8k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 16:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map16k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 32:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map32k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;

                case 64:
                    pagearrayindex = pagenum / 32;
                    pagearrayoffset = pagenum % 32;
                    map64k[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));

                    break;
                default:
                    direction &= ~2;
                    break;
            }

            ++pagenum;
        }
    }
}
/*{
  unsigned pagearrayindex = pagenum / 32;
  unsigned pagearrayoffset = pagenum % 32;

  map[pagearrayindex] &= ~(1 << (31 - pagearrayoffset));
  }*/

void pfree(void *ptr)
{
    unsigned lptr = (unsigned)ptr;
    unsigned pnum = lptr / 0x1000;
    freepagenum(4, pnum);
}

void *palloc(unsigned int size)
{
    unsigned pagenum = 0;

    if(size <= 0x1000)
    {
        pagenum = findfreepage(map4k, map4klength);
        allocpagenum(4, pagenum);
        return (void*)(0x1000 * pagenum);
    }
    else if(size <= 0x2000)
    {
        pagenum = findfreepage(map8k, map8klength);
        allocpagenum(8, pagenum);
        return (void*)(0x2000 * pagenum);
    }
    else if(size <= 0x4000)
    {
        pagenum = findfreepage(map16k, map16klength);
        allocpagenum(16, pagenum);
        return (void*)(0x4000 * pagenum);
    }
    else if(size <= 0x8000)
    {
        pagenum = findfreepage(map32k, map32klength);
        allocpagenum(32, pagenum);
        return (void*)(0x8000 * pagenum);
    }
    else if(size <= 0x10000)
    {
        pagenum = findfreepage(map64k, map64klength);
        allocpagenum(64, pagenum);
        return (void*)(0x10000 * pagenum);
    }

    return 0;
}

long long findfreepage(unsigned *map, long long maplength)
{
    long long i;

    for(i = 0; i < maplength; ++i)
    {
        if(~(map[i] & 0xFFFFFFFF))
            break;
    }

    if(i == maplength)
        return -1;

    unsigned mask = (1 << 31);
    unsigned short maskindex = 0;

    while(mask & map[i])
    {
        ++maskindex;
        mask >>= 1;
    }

    return (i * 32) + maskindex;
}
