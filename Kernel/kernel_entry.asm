[bits 32]

section .entry
[extern main]
[extern kpanic]

[global start_kernel]
start_kernel:
    mov [MemSize], bx   ; Get memory size from bx
    call main		        ; calls kernel function main()

    jmp $

section .text
%include "../CPU/Interrupts/interrupt.asm"
%include "../Memory/Paging/paging.asm"


section .data
    global MemSize
    MemSize: db 0, 0

section .rodata		    ; read only data

%include "IncBins.asm"	; including binaries

