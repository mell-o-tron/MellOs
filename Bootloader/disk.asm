disk_read:
	mov ah, 0x02
	mov al, dh
	mov ch, 0x00
	mov cl, 0x02
	mov dh, 0x00
	mov dl, [BOOT_DISK]
	int 0x13
	
	jc disk_read_error
	
	cmp ah, 0x00
	jne disk_read_error
	
	mov bx, ax
	call print_dec
    ret

drv_params_entry equ 0x5200
    
drive_parameters:	; input: dh disk number
	mov ah, 0x08
	int 13
	jc disk_read_error
	
	mov [drv_params_entry], bl			; CMOS drive type
	mov [drv_params_entry + 1], ch		; cylinders
	mov [drv_params_entry + 2], dh		; sides
	mov [drv_params_entry + 3], dl		; drives attached
	ret
    
disk_read_error:
	push ax
	mov bx, DISK_ERROR
	call print_string
	pop ax
	mov bh, ah
	call print_dec
	jmp $

BOOT_DISK:
	db 0x00

DISK_ERROR:
	db "DSKERR", 0x0A, 0x0D, 0x00
