#include "elf.h"
#include "readfat.h"
#include "modules/kprint.h"

#define KERNEL_OFFSET (void*)0x4000
#define KERNEL_NAME "SYSTEM  BIN"

typedef void(*kmain_function)();

void *get_entry_point32(void *elf_start)
{
	Elf32_Ehdr *header = (Elf32_Ehdr*)elf_start;

	return (void*)(header->e_entry);
}

#ifdef RIX_X86_64
void *get_entry_point64(void *elf_start)
{
	Elf64_Ehdr *header = (Elf64_Ehdr*)elf_start;

	return (void*)(header->e_entry);
}
#endif

void *get_entry_point(void *elf_start)
{
	/*	Elf32 and Elf64 start the same so cast to Elf32 header to
		find out if 32-bit or 64-bit. */

	unsigned char elf_class = ((Elf32_Ehdr*)elf_start)->e_ident[EI_CLASS];

	if(elf_class == ELFCLASS32)
		return get_entry_point32(elf_start);

#ifdef RIX_X86_64
	else if(elf_class == ELFCLASS64)
		return get_entry_point64(elf_start);
#endif

	else
		return 0;
}

void load_kernel(void)
{
	writes("reading at line __LINE__");

	/*	Read file from FAT into memory. */
	read_file_to(KERNEL_NAME, KERNEL_OFFSET);

	/*	Find entry point. */
	kmain_function entry_point = (kmain_function)get_entry_point(KERNEL_OFFSET);

	/*	Jump to entry point (passing in required info). */
	//entry_point();

	__asm__ (
			"jmp *%%eax"
			: /* no output registers */
			: "a" (entry_point)
			:
		);
}