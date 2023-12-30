disk_read:
	; other stuff should be set by the
	mov ah, 0x02
	mov dl, [BOOT_DISK]
	int 0x13
	
	jc disk_read_error
	
	cmp ah, 0x00
	jne disk_read_error

	xor ah, ah
	mov bx, ax
	call print_dec

	mov bx, DISK_SUCC
	call print_string

    ret

drv_params_entry equ 0x5200
    
drive_parameters:	; input: dl disk number		TODO FIXME
;
; 	;AH = 08h
; 	;DL = drive (bit 7 set for hard disk)
; 	;ES:DI = 0000h:0000h to guard against BIOS bugs
;
; 	mov ah, 0x08
; 	;xor es, es
; 	xor di, di
; 	int 13
; 	jc disk_param_error		;CF set on error
; 	cmp ah, 0x0
; 	jne disk_param_error
; 	;Return:
; 	;CF clear if successful
; 	;AH = 00h
; 	;AL = 00h on at least some BIOSes
; 	;BL = drive type (AT/PS2 floppies only) (see #00242)
; 	;CH = low eight bits of maximum cylinder number
; 	;CL = maximum sector number (bits 5-0)
; 	;high two bits of maximum cylinder number (bits 7-6)
; 	;DH = maximum head number
; 	;DL = number of drives
; 	;ES:DI -> drive parameter table (floppies only)
;
; 	mov [drv_params_entry], bl
; 	mov [drv_params_entry + 1], ch
; 	mov [drv_params_entry + 2], cl
; 	mov [drv_params_entry + 3], dh
; 	mov [drv_params_entry + 4], dl
	ret
    
disk_read_error:
	mov bx, DISK_ERROR
	jmp disk_error

disk_param_error:
	mov bx, DISK_PARAM_ERROR
	jmp disk_error


disk_error:
	push ax
	call print_string
	pop ax
	mov bh, ah
	call print_dec
	jmp $

BOOT_DISK:
	db 0x00

DISK_ERROR:
	db "dsk_read_err", 0x0A, 0x0D, 0x00

DISK_PARAM_ERROR:
	db "dsk_param_err", 0x0A, 0x0D, 0x00

DISK_SUCC:
	db " - dsk_succ", 0x0A, 0x0D, 0x00
