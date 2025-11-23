OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(_kernel_start)

KLOAD = 0x00100000;
KHIGH = 0xC0000000;

/* Program headers: R=4, W=2, X=1 */
PHDRS {
  lowhdr   PT_LOAD FLAGS(5);   /* R+X for low code */
  lowdata  PT_LOAD FLAGS(6);   /* R+W for low data, bss */

  texthdr  PT_LOAD FLAGS(5);   /* R+X for high .text */
  rodatahdr PT_LOAD FLAGS(4);  /* R, no exec for .rodata */
  datahdr  PT_LOAD FLAGS(6);   /* R+W for high .data */
  bsshdr   PT_LOAD FLAGS(6);   /* R+W for high .bss  */
  /*todo: tls*/
}

/* Monotonic LMA cursor */
__lma = KLOAD;

SECTIONS
{
  /* ---------- LOW phase ---------- */
  . = KLOAD;

  /* one low RX segment */
  .lowrx ALIGN(4K) : AT(__lma) {
    KEEP(*(.multiboot))
    . = ALIGN(4K);
    *(.entry*) *(.low.text*)
  } :lowhdr
  __lma = ALIGN(LOADADDR(.lowrx) + SIZEOF(.lowrx), 4K);


  .low.data ALIGN(4K) : AT(__lma) { *(.low.rodata*) *(.low.data*) }
  :lowdata
  __lma = ALIGN(ADDR(.low.data) + SIZEOF(.low.data), 4K);

  .low.bss ALIGN(4K) (NOLOAD) : {
    *(COMMON) *(.low.bss*)
  } :lowdata
  __lma = ALIGN(ADDR(.low.bss) + SIZEOF(.low.bss), 4K);

  /* ---------- HIGH phase ---------- */
  . = KHIGH;

  .text ALIGN(4K) : AT(__lma) { *(.text*) } :texthdr

  PROVIDE(__text_pa_start = LOADADDR(.text));
  PROVIDE(__text_va_start = ADDR(.text));
  __lma = ALIGN(__lma + SIZEOF(.text), 4K);
  PROVIDE(__text_pa_end = LOADADDR(.text) + SIZEOF(.text));
  PROVIDE(__text_va_end = ADDR(.text) + SIZEOF(.text));

  .rodata ALIGN(4K) : AT(__lma) { *(.rodata*) } :rodatahdr

  PROVIDE(__rodata_pa_start = LOADADDR(.rodata));
  __lma = ALIGN(__lma + SIZEOF(.rodata), 4K);
  PROVIDE(__rodata_pa_end = LOADADDR(.rodata) + SIZEOF(.rodata));
  PROVIDE(__rodata_va_start = ADDR(.rodata));
  PROVIDE(__rodata_va_end = ADDR(.rodata) + SIZEOF(.rodata));

  .data ALIGN(4K) : AT(__lma) { *(.data*) } :datahdr

  PROVIDE(__data_pa_start = LOADADDR(.data));
  __lma = ALIGN(__lma + SIZEOF(.data), 4K);
  PROVIDE(__data_pa_end = LOADADDR(.data) + SIZEOF(.data));
  PROVIDE(__data_va_start = ADDR(.data));
  PROVIDE(__data_va_end = ADDR(.data) + SIZEOF(.data));

  .bss ALIGN(4K) (NOLOAD) : { *(.bss*) } :bsshdr

  __lma = ALIGN(__lma + SIZEOF(.bss), 4K);
  PROVIDE(__bss_pa_start = LOADADDR(.bss));
  PROVIDE(__bss_pa_end = LOADADDR(.bss) + SIZEOF(.bss));
  PROVIDE(__bss_va_start = ADDR(.bss));
  PROVIDE(__bss_va_end = ADDR(.bss) + SIZEOF(.bss));

  /* Useful labels */
  PROVIDE(__kload_start = KLOAD);
  PROVIDE(__khigh_base  = KHIGH);
  PROVIDE(__image_lma_end = __lma);
}
