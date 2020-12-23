section .text
[bits 32]

[extern main]
call main		; calls kernel function main()

jmp $

%include "../CPU/interrupt.asm"


section .rodata		; read only data

%include "IncBins.asm"	; including binaries

