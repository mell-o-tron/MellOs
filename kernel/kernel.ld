ENTRY(start_kernel)
OUTPUT_FORMAT(elf32-i386)
SECTIONS {
    /* Where the kernel starts */
    . = 0x400000;

    /* Make sure the entry point is the very first thing that is placed in the binary */
    .entry : { *(.entry) }
    .text : { *(.text) }

    .rodata : { *(.rodata) }
    .data : { *(.data) }

    .bss : {
        *(COMMON)
        *(.bss)
    }
}