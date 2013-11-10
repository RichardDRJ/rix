#include "harddiskdriver.h"
#include "readfat.h"
#include "string.h"
#include "palloc.h"
#include "system.h"
#include "kprint.h"

FILE *readfiletoFILE(char *shortname)
{
    FILE *retfile = palloc(sizeof(FILE));
    struct bpb *header = palloc(512);
    
    readblock(0, (unsigned char*)header, 1, 0);
    header = (struct bpb*)((unsigned char*)header + 3);
    
    fileentry_t *initEntry;
    initEntry = getinitialentry(shortname, header);

    unsigned int fatSize = (header->sectorsPerFAT * header->sectorSize);
    unsigned char *fat = readFAT(header, fatSize);

    retfile->currentreadoffset = 0;
    retfile->currentwriteoffset = 0;
    retfile->stream = palloc(initEntry->sizebytes);
    retfile->length = initEntry->sizebytes;
    unsigned short clusternum = initEntry->startcluster;
    unsigned int maxnumclusters = header->sectorsPerFAT * header->sectorSize / 16;

    while(clusternum && clusternum < maxnumclusters)
    {
        readcluster(clusternum, header, retfile->stream + retfile->currentwriteoffset);
        retfile->currentwriteoffset += (header->sectorsPerCluster * header->sectorSize);
        clusternum = nextcluster(clusternum, fat);
    }

    pfree(header);
    pfree(fat);

    return retfile;
}

unsigned char *readfile(char *shortname)
{
    struct bpb *header = palloc(512);
    
    readblock(0, (unsigned char*)header, 1, 0);
    header = (struct bpb*)((unsigned char*)header + 3);
    
    fileentry_t *initEntry;
    initEntry = getinitialentry(shortname, header);

    unsigned int fatSize = (header->sectorsPerFAT * header->sectorSize);
    unsigned char *fat = readFAT(header, fatSize);
    

    unsigned char *buffer = palloc(initEntry->sizebytes);
    unsigned char *retval = buffer;
    unsigned short clusternum = initEntry->startcluster;

    unsigned int maxnumclusters = header->sectorsPerFAT * header->sectorSize / 16;

    while(clusternum < maxnumclusters)
    {
        readcluster(clusternum, header, buffer);
        buffer += (header->sectorsPerCluster * header->sectorSize);
        clusternum = nextcluster(clusternum, fat);
    }

    pfree(header);
    pfree(fat);

    return retval;
}

unsigned short nextcluster(unsigned short currcluster, unsigned char *fat)
{
    return *(unsigned short*)((unsigned int)fat + (currcluster) * 2);
}

unsigned char *readFAT(struct bpb *header, unsigned int fatSize)
{
    unsigned int FATblock = header->reservedSectors;
    unsigned char *retmem = palloc(fatSize);
    unsigned char *currRead;
    currRead = retmem;

    fatSize -= 512;

    while(fatSize > 0 && FATblock < header->sectorsPerFAT)
    {
        readblock(FATblock++, currRead, 1, 0);
        currRead += 512;
        fatSize -= 512;
    }
    return retmem;
}

void readcluster(unsigned int clusternum, struct bpb *header, unsigned char *buffer)
{
    unsigned int datastart = header->reservedSectors + header->numberOfFATs * header->sectorsPerFAT + (header->rootEntries * 32) / header->sectorSize + (clusternum - 2) * header->sectorsPerCluster;

    unsigned char sectorsleft = header->sectorsPerCluster;

    while(sectorsleft --> 0)
    {
        readblock(datastart++, buffer, 1, 0);
        buffer += 512;
    }
}

fileentry_t *getinitialentry(char* shortname, struct bpb *header)
{
    unsigned int rootblock = (header->reservedSectors + header->numberOfFATs * header->sectorsPerFAT);

    fileentry_t *rootentries = palloc(16 * sizeof(fileentry_t));
    unsigned int entryindex;

    for(entryindex = 0; entryindex < header->rootEntries; ++entryindex)
    {
        if(!(entryindex % (16)))
        {
            readblock(rootblock++, (unsigned char*)rootentries, 1, 0);
        }

        char currshortname[12];

        unsigned short i;
        for(i = 0; i < 11; ++i)
            currshortname[i] = rootentries[entryindex % 16].shortname[i];
        currshortname[11] = 0;

        if(!(strcmp(shortname, currshortname)))
            break;
    }

    return &(rootentries[entryindex % 16]);;
}
