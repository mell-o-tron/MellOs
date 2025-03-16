[org 0x7e00]
[bits 16]

STAGE_3_LOCATION equ 0x8200

mov bx, STAGING
call print_string

;read the stage 3 bootloader (60 sectors)
mov bx, STAGE_3_LOCATION
mov al, 13			; read 13 sectors
mov ch, 0x00		; from cylinder 0
mov dh, 0x00		; from head 0
mov cl, 0x04		; from sector 4 (counting from 1)
call disk_read

; WHATEVER IS PUT INTO BX HERE WILL BE WRITTEN INTO THE MEMSIZE VARIABLE!
; Can be adapted to pass any information to the kernel without storing it in memory
 call detect_cont_memory

; the result of these calls is stored in memory, for the kernel to fetch later
call memory_detection
call upper_mem_map
; 	; disk parameters
; %ifdef VGA_VESA
call vbe_setup

; %endif

mov dl, 0xA0
call drive_parameters

jmp enter_protected_mode

jmp $

%include"print_string.asm"
%include"print_dec.asm"
%include"vbe_modeset.asm"
%include"disk.asm"
%include"memory.asm" ; old memory detection code
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
	jmp STAGE_3_LOCATION
    jmp $

STAGING:
	db "Separation Confirmed.", 0x0A, 0x0D, 0x00

times 1024-($-$$) db 0x00
