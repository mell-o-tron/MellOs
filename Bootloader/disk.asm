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
	db "Error reading from disk!", 0x0A, 0x0D, 0x00
