section .text
[bits 32]

global MemSize
    MemSize: db 0, 0
    mov [MemSize], bx   ; Get memory size from bx

[extern main]
[extern kpanic]
call main		        ; calls kernel function main()

jmp $

%include "../CPU//Interrupts/interrupt.asm"
%include "../Memory/Paging/paging.asm"


section .rodata		    ; read only data

%include "IncBins.asm"	; including binaries

