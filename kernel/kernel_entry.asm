%macro OUT_SERIAL 1
    mov al, %1
    out 0x3f8, al
%endmacro

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

; kernel_entry.asm
bits 32
global _kernel_start
extern main    
extern kpanic

section .entry
_kernel_start:
    cli
    
    lgdt [GDT_descriptor]
    
    mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp
        
    xor ebp, ebp
    
    call main
    jmp $
    
GDT_start:
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start

init_serial:
    mov dx, 0x3f8
    mov al, 0x00
    out dx, al         ; disable interrupts
    inc dx
    mov al, 0x80
    out dx, al         ; enable DLAB
    dec dx
    mov al, 0x03
    out dx, al         ; divisor low
    inc dx
    mov al, 0x00
    out dx, al         ; divisor high
    dec dx
    mov al, 0x03
    out dx, al         ; 8 bits, no parity, one stop
    inc dx
    mov al, 0x03
    out dx, al         ; enable FIFO, clear them
    dec dx
    mov al, 0x0b
    out dx, al         ; IRQs enabled, RTS/DSR set
    ret
    
section .text
    
%include "../cpu/interrupts/interrupt.asm"
%include "../memory/paging/paging.asm"

section .data

section .rodata		    ; read only data

%include "IncBins.asm"	; including binaries
