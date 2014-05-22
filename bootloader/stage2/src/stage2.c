#include "stdlib/elf.h"
#include "drivers/readfat.h"
#include "modules/kprint.h"
#include "stdlib/system.h"

#define KERNEL_OFFSET (void*)0x100000
#define KERNEL_NAME "SYSTEM  BIN"

typedef void(*kmain_function)();

void *get_entry_point32(Elf32_Ehdr *header)
{
	return (void*)(header->e_entry);
}

#ifdef RIX_X86_64
void *get_entry_point64(Elf64_Ehdr *header)
{
	return (void*)(header->e_entry);
}
#endif

void follow_program_header32(Elf32_Ehdr *header)
{
	Elf32_Phdr *header_array = (void*)header + header->e_phoff;

	Elf32_Half num_headers = header->e_phnum;

	for(Elf32_Half i = 0; i < num_headers; ++i)
	{
		Elf32_Phdr *curr_header = &header_array[i];
		switch(curr_header->p_type)
		{
			case PT_LOAD: {

				void *load_address = (void*)curr_header->p_vaddr;
				memcpy(load_address, (void*)((void*)header + curr_header->p_offset), curr_header->p_filesz);

				//	TODO: Set page table entries for user programs based on p_flags.

				} break;
			default:
				break;
		}
	}
}

#ifdef RIX_X86_64
void follow_program_header64(Elf64_Ehdr *header)
{
	Elf64_Phdr *header_array = (void*)header + header->e_phoff;

	Elf64_Half num_headers = header->e_phnum;

	for(Elf64_Half i = 0; i < num_headers; ++i)
	{
		Elf64_Phdr *curr_header = &header_array[i];
		switch(curr_header->p_type)
		{
			case PT_LOAD: {

				void *load_address = (void*)curr_header->p_vaddr;
				memcpy(load_address, (void*)((void*)header + curr_header->p_offset), curr_header->p_filesz);

				//	TODO: Set page table entries for user programs based on p_flags.

				} break;
			default:
				break;
		}
	}
}
#endif

void *get_entry_point(void *elf_start)
{
	/*	Elf32 and Elf64 start the same so cast to Elf32 header to
		find out if 32-bit or 64-bit. */

	unsigned char elf_class = ((Elf32_Ehdr*)elf_start)->e_ident[EI_CLASS];

	if(elf_class == ELFCLASS32)
		return get_entry_point32((Elf32_Ehdr*)elf_start);

#ifdef RIX_X86_64
	else if(elf_class == ELFCLASS64)
		return get_entry_point64((Elf64_Ehdr*)elf_start);
#endif

	else
		return 0;
}

void follow_program_header(void *elf_start)
{
	/*	Elf32 and Elf64 start the same so cast to Elf32 header to
		find out if 32-bit or 64-bit. */

	unsigned char elf_class = ((Elf32_Ehdr*)elf_start)->e_ident[EI_CLASS];

	if(elf_class == ELFCLASS32)
		follow_program_header32((Elf32_Ehdr*)elf_start);

#ifdef RIX_X86_64
	else if(elf_class == ELFCLASS64)
		follow_program_header64((Elf64_Ehdr*)elf_start);
#endif
}

void load_kernel(void)
{
	writes("reading at line __LINE__");

	/*	Read file from FAT into memory. */
	read_file_to(KERNEL_NAME, KERNEL_OFFSET);

	follow_program_header(KERNEL_OFFSET);

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