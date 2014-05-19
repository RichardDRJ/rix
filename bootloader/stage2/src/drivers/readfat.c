#include "harddiskdriver.h"
#include "readfat.h"
#include "string.h"
#include "system.h"
#include "kprint.h"

char header_space[512];
fileentry_t rootentries[16];

void read_file_to(char *shortname, void *dest)
{
	struct bpb *header = (struct bpb*)header_space;

	readblock(0, (unsigned char*)header, 1, 0);
	header = (struct bpb*)((unsigned char*)header + 3);

	fileentry_t *initEntry;
	initEntry = getinitialentry(shortname, header);

	/*  TODO: Make this more robust. Find a better place to put the FAT. */
	unsigned int fatSize = (header->sectorsPerFAT * header->sectorSize);
	unsigned char *fat = 0;
	read_FAT_to(header, fatSize, fat);

	unsigned char *buffer = dest;
	unsigned short clusternum = initEntry->startcluster;

	unsigned int maxnumclusters = header->sectorsPerFAT * header->sectorSize / 16;

	while(clusternum < maxnumclusters)
	{
		readcluster(clusternum, 1 header, buffer);
		buffer += (header->sectorsPerCluster * header->sectorSize);
		clusternum = nextcluster(clusternum, fat);
	}
}

unsigned short nextcluster(unsigned short currcluster, unsigned char *fat)
{
	return *(unsigned short*)((unsigned int)fat + (currcluster) * 2);
}

/*  TODO: Return a status code. */
void read_FAT_to(struct bpb *header, unsigned int fatSize, void *dest)
{
	unsigned int FATblock = header->reservedSectors;
	unsigned char *currRead;
	currRead = dest;

	fatSize -= 512;

	readblock(FATblock++, currRead, header->sectorsPerFAT, 0);
}

void readclusters(unsigned int cluster, unsigned int numclusters, struct bpb *header, unsigned char *buffer)
{
	unsigned int datastart = header->reservedSectors + header->numberOfFATs * header->sectorsPerFAT + (header->rootEntries * 32) / header->sectorSize + (cluster - 2) * header->sectorsPerCluster;

	unsigned char sectorsleft = header->sectorsPerCluster * numclusters;

	while(sectorsleft > 0)
	{
		readblock(datastart++, buffer, header->sectorsPerCluster, 0);
		buffer += 512 * header->sectorsPerCluster;

		sectorsleft -= header->sectorsPerCluster;
	}
}

fileentry_t *getinitialentry(char* shortname, struct bpb *header)
{
	unsigned int rootblock = (header->reservedSectors + header->numberOfFATs * header->sectorsPerFAT);
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

	return &(rootentries[entryindex % 16]);
}
