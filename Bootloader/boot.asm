[org 0x7c00]
[bits 16]

STAGE_2_LOC equ 0x7e00

_main16:
	;save boot disk number
	mov [BOOT_DISK], dl

	;set up segment registers
	cli
	mov ax, 0x00
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	sti
    
	
	;clear the screen
	mov ah, 0x00
	mov al, 0x03
	int 0x10

	xor bx, bx
    mov es, bx

	;read the stage 2 bootloader (2 sectors)
	mov bx, STAGE_2_LOC
	mov al, 0x02		; read two sectors
	mov ch, 0x00		; from cylinder 0
	mov cl, 0x02		; from sector 2 (counting from 1)
	mov dh, 0x00		; from head 0
	call disk_read

	jmp STAGE_2_LOC

	jmp $

%include"print_string.asm"
%include"print_dec.asm"
%include"disk.asm"



times 510-($-$$) db 0x00
dw 0xAA55
