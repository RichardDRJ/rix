#include "systemcall.h"
#include "system.h"
#include "kprint.h"
#include "palloc.h"
#include "readfat.h"
#include "stdin.h"
#include "file.h"

void *systable[NUMSYSFUNCTIONS];

void systemCall(struct regs*);
void print(struct regs*);
void printc(struct regs*);
void sbrk(struct regs*);
void exit(struct regs*);
void intpalloc(struct regs*);
void intreadfile(struct regs*);
void intsetupstdin(struct regs *r);
void intpfree(struct regs *r);
void intreadfiletofile(struct regs *r);

void initSystemCalls(void)
{
    memset(systable, 0, NUMSYSFUNCTIONS * sizeof(void*));
    systable[PRINT] = &print;
    systable[PRINTC] = &printc;
    systable[SBRK] = &sbrk;
    systable[EXIT] = &exit;
    systable[PALLOC] = &intpalloc;
    systable[READFAT] = &intreadfile;
    systable[STDINSETUP] = &intsetupstdin;
    systable[PFREE] = &intpfree;
    systable[READTOFILE] = &intreadfiletofile;
}

void systemCall(struct regs *r)
{
    void (*sysfunction)(struct regs*);
    sysfunction = systable[r->eax];
    (*sysfunction)(r);
}

/*  ebx = pointer to string to write. */
void print(struct regs *r)
{
    writes((char*)r->ebx);
}

/*  ebx = char to write. */
void printc(struct regs *r)
{
    writec((char)r->ebx);
}

/*  ebx = size to allocate. ecx = address of pointer to new memory. */
void intpalloc(struct regs *r)
{
    *(unsigned char**)(r->ecx) = (unsigned char*)palloc(r->ebx);
}

/*  ebx = location to free. */
void intpfree(struct regs *r)
{
    pfree((void*)r->ebx);
}


/*  ebx = size to increase by. ecx = address of pointer to new memory. */
void sbrk(__attribute__((__unused__)) struct regs *r)
{
    
}

/*  ebx = exit code. */
void exit(__attribute__((__unused__)) struct regs *r)
{

}

/*  ebx = pointer to file name, ecx = pointer to file pointer output. */
void intreadfile(struct regs *r)
{
    *(unsigned char**)(r->ecx) = readfile((char*)(r->ebx));
}

/*  ebx = pointer to file name, ecx = pointer to file pointer output. */
void intreadfiletofile(struct regs *r)
{
    *(FILE**)(r->ecx) = readfiletoFILE((char*)(r->ebx));
}

void intsetupstdin(struct regs *r)
{
    *(FILE**)(r->ebx) = getstdinstream();
}
