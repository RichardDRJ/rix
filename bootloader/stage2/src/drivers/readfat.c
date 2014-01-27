#include "harddiskdriver.h"
#include "readfat.h"
#include "string.h"
#include "system.h"

char header_space[512];

void read_file_to(char *shortname, void *dest)
{
    struct bpb *header = header_space;
    
    readblock(0, (unsigned char*)header, 1, 0);
    header = (struct bpb*)((unsigned char*)header + 3);
    
    fileentry_t *initEntry;
    initEntry = getinitialentry(shortname, header);

    /*  TODO: Make this more robust. Find a better place to put the FAT. */
    unsigned int fatSize = (header->sectorsPerFAT * header->sectorSize);
    unsigned char *fat = read_FAT_to(header, 0);

    unsigned char *buffer = dest;
    unsigned char *retval = buffer;
    unsigned short clusternum = initEntry->startcluster;

    unsigned int maxnumclusters = header->sectorsPerFAT * header->sectorSize / 16;

    while(clusternum < maxnumclusters)
    {
        readcluster(clusternum, header, buffer);
        buffer += (header->sectorsPerCluster * header->sectorSize);
        clusternum = nextcluster(clusternum, fat);
    }
}

unsigned short nextcluster(unsigned short currcluster, unsigned char *fat)
{
    return *(unsigned short*)((unsigned int)fat + (currcluster) * 2);
}

unsigned char *read_FAT_to(struct bpb *header, unsigned int fatSize)
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

/*  TODO: Return a status code. */
void read_FAT_to(struct bpb *header, void *dest)
{
    unsigned int FATblock = header->reservedSectors;
    unsigned char *currRead;
    currRead = dest;

    fatSize -= 512;

    while(fatSize > 0 && FATblock < header->sectorsPerFAT)
    {
        readblock(FATblock++, currRead, 1, 0);
        currRead += 512;
        fatSize -= 512;
    }
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
