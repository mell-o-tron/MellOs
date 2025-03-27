[bits 32]

section .entry
[extern main]

[global start_stage_3]
start_stage_3:
    call main		        ; calls stage 3 loader function main()

    mov eax, 0x400000       ; kernel entry point
    jmp eax
    jmp $

section .text

section .data

section .rodata		    ; read only data
