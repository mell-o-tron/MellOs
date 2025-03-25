ENTRY(start_stage_3)
OUTPUT_FORMAT(elf32-i386)

MEMORY
{
    ram (rwx) : ORIGIN = 0x8200, LENGTH = 0x8000000      /* Usable RAM */
    reserved_VGA_FB (r) : ORIGIN = 0xA0000, LENGTH = 0x20000  /* Reserved range */
}

SECTIONS {
    /* Where the stage 3 loader resides */
    . = 0x8200;

    /* Make sure the entry point is the very first thing that is placed in the binary */
    .entry : { *(.entry) }
    .text : { *(.text) }

    .rodata : { *(.rodata) }
    .data : { *(.data) }

    .bss : {
        *(COMMON)
        *(.bss)
    }
    .vga_fb : { *(.vga_fb) } > reserved_VGA_FB
}