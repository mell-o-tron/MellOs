[org 0x7C00]
[bits 16]

KERNEL_LOCATION equ 0x1000

_main16:
	;save boot disk number
	mov [BOOT_DISK], dl

	;clear the screen
	mov ah, 0x00
	mov al, 0x03
	int 0x10

	;read 50 sectors at kernel location
	mov bx, KERNEL_LOCATION
	mov dh, 50
	call disk_read
	
	;disk read end label as return doesn't work
	disk_read_end:

	;set up segment registers
	cli
	mov ax, 0x00
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	sti

	jmp enter_protected_mode

%include"print_string.asm"
%include"print_dec.asm"
%include"disk.asm"
%include"memory.asm"
%include"protected_mode.asm"
%include"gdt.asm"

[bits 32]

_main32:
	;set up segment registers again
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;set up the stack
	mov ebp, 0x90000
	mov esp, ebp
	
	;enable the A20 line
	in al, 0x92
	or al, 0x02
	out 0x92, al

	;jump to kernel location
	jmp KERNEL_LOCATION
	
%include"print_string32.asm"

times 510-($-$$) db 0x00
dw 0xAA55
