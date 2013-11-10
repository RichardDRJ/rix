ENTRY (setupstack)
OUTPUT_FORMAT(binary)

SECTIONS
{
    PROVIDE (_executable_start = 0x1000); . = 0x1000;

    .text : AT (0x1000)
    {
        *(.text)
    }
    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
    }

    PROVIDE (_etext = .);
}
