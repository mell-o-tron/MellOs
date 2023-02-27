section .text
[bits 32]

mov [MemSize], bx   ; Get memory size from bx

[extern main]
[extern kpanic]
call main		        ; calls kernel function main()

jmp $

%include "../CPU//Interrupts/interrupt.asm"
%include "../Memory/Paging/paging.asm"


section .data
    global MemSize
    MemSize: db 0, 0

section .rodata		    ; read only data

%include "IncBins.asm"	; including binaries

