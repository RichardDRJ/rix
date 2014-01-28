#include "elf.h"
#include "readfat.h"

#define KERNEL_OFFSET (void*)0x100000
#define KERNEL_NAME "kernel.bin"

void load_kernel(void)
{
	/*	Read file from FAT into memory. */
	read_file_to(KERNEL_NAME, KERNEL_OFFSET);
	
	/*	Find entry point. */
	/*	Jump to entry point (passing in required info). */
}