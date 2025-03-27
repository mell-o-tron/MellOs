[org 0x7e00]
[bits 16]

STAGE_3_LOCATION equ 0x8200

mov bx, STAGING
call print_string

;read the kernel (60 sectors)
mov bx, STAGE_3_LOCATION
mov al, 63			; read 128 sectors
mov ch, 0x00		; from cylinder 0
mov dh, 0x00		; from head 0
mov cl, 0x04		; from sector 4 (counting from 1)
call disk_read

; push es
; mov ax, 0x1000
; mov es, ax
; mov bx, 0x0000
; mov al, 128			; read 128 sectors
; mov ch, 0x00		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 031		; from sector 4 (counting from 1)
; call disk_read

; mov ax, 0x2000
; mov es, ax
; mov bx, 0x0000
; mov al, 128			; read 128 sectors
; mov ch, 0x02		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 015		; from sector 4 (counting from 1)
; call disk_read
; mov bx, STAGE_3_LOCATION + (0x4800 * 3)
; mov al, 20			; read 128 sectors
; mov ch, 0x01		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 0x03		; from sector 4 (counting from 1)
; call disk_read
; ; mov bx, STAGE_3_LOCATION + (0x4800 * 3) + 0x1400
; ; mov al, 10			; read 128 sectors
; ; mov ch, 0x01		; from cylinder 0
; ; mov dh, 0x01		; from head 0
; ; mov cl, 0x0a		; from sector 4 (counting from 1)
; ; call disk_read
; mov ax, 0x2000
; mov es, ax
; mov bx, STAGE_3_LOCATION + (0x4800 * 3) + 0x2800
; mov al, 128			; read 128 sectors
; mov ch, 0x01		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 0x15		; from sector 4 (counting from 1)
; call disk_read
; mov bx, STAGE_3_LOCATION + 0x4000 + 0x4800
; mov al, 36			; read 128 sectors
; mov ch, 0x01		; from cylinder 0
; mov dh, 0x00		; from head 0
; mov cl, 0x01		; from sector 4 (counting from 1)
; call disk_read
; mov bx, STAGE_3_LOCATION + 0x4000 + 0x4800 + 0x4800
; mov al, 24			; read 128 sectors
; mov ch, 0x01		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 0x01		; from sector 4 (counting from 1)
; call disk_read
; mov ax, 0x2000
; mov es, ax
; mov bx, STAGE_3_LOCATION
; mov al, 12			; read 128 sectors
; mov ch, 0x01		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 24		; from sector 4 (counting from 1)
; call disk_read

; pop es
; jmp $

; push es
; mov ax, 0x2000
; mov es, ax
; mov bx, STAGE_3_LOCATION
; mov al, 1			; read 128 sectors
; mov ch, 0x01		; from cylinder 0
; mov dh, 0x01		; from head 0
; mov cl, 0x16		; from sector 4 (counting from 1)
; call disk_read
; pop es

; WHATEVER IS PUT INTO BX HERE WILL BE WRITTEN INTO THE MEMSIZE VARIABLE!
; Can be adapted to pass any information to the kernel without storing it in memory
 call detect_cont_memory

; the result of these calls is stored in memory, for the kernel to fetch later
call memory_detection
call upper_mem_map
; 	; disk parameters
mov dl, 0xA0
call drive_parameters

jmp enter_protected_mode

jmp $

%include"print_string.asm"
%include"print_dec.asm"
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
