[bits 32]

section .entry
[extern main]
[extern kpanic]

[global start_kernel]
start_kernel:
    call main		        ; calls kernel function main()

    jmp $

section .text
%include "../cpu/interrupts/interrupt.asm"
%include "../memory/paging/paging.asm"

section .data

section .rodata		    ; read only data

%include "IncBins.asm"	; including binaries
