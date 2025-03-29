[bits 32]

section .entry
[extern main]

KERNEL_LOCATION equ 0x400000

[global start_stage_3]
start_stage_3:
    call main		                ; calls stage 3 loader function main()

    mov eax, KERNEL_LOCATION        ; kernel entry point
    jmp eax
    jmp $

section .text

section .data

section .rodata		                ; read only data
