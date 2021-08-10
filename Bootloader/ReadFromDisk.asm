
			 ; NUMBER OF SECTORS TO BE LOADED IS IN DH ALREADY.
			 ; ES:BX IS SET AS THE LOCATION TO READ FROM
readDisk:
	
	;push dx	 ; needed to check number of segments loaded

	mov ah, 0x02	 ; BIOS read from disk routine

	mov al, dh  	 ; dh is the number of segments we want to read

	mov ch, 0x00	 ; track / cylinder number 0
	mov dh, 0x00	 ; head 0
	mov cl, 0x02	 ; start reading from sector 2 (after boot sector)
	
	mov dl, [BOOT_DISK]
	int 0x13	 ; read from disk interrupt

	jc diskError	 ; jump to error if Carry Flag HIGH
	
	cmp ah, 0	 ; AH contains the status. If it's 0, it's alright
	jne diskError
	
	mov bx, ax
	call PrintDecimal	; Print Number of sectors loaded

	jmp AFTER_DISK_READ	; I have no idea why return is not working, I'll figure it out
	ret



diskError:
	push ax
	mov bx, DISK_ERROR_MSG
	
	call PrintString
	pop ax
	mov bh, ah
	call PrintDecimal
	jmp $

sectorError:
	
	mov bx, SECTOR_ERROR_MSG
	call PrintString
	jmp $

BOOT_DISK: db 0

DISK_ERROR_MSG:
	db "Disk Error!",10, 13, 0

SECTOR_ERROR_MSG:
	db "Sector Error!",10, 13, 0


