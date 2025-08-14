ENTRY(_kernel_start)

SECTIONS
{
  . = 0x00100000;

  .multiboot :
  {
    *(.multiboot)
  }
  
  .entry :
  {
    *(.entry*)
  }
  
  .text :
  {
    *(.text*)
  }

  .rodata :
  {
    *(.rodata*)
  }

  .data :
  {
    *(.data*)
  }

  .bss :
  {
    *(COMMON)
    *(.bss*)
  }
}

