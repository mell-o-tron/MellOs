; INT 0x13: Disk I/O
; AH = 0x02 - Read Sectors From Drive
; AL = Sectors To Read Count
; CH = Cylinder Number
; CL = Sector Number
; DH = Head Number
; DL = Drive Number
; ES:BX = Buffer Address

; ERRORS: https://stanislavs.org/helppc/int_13-1.html
disk_read:
	; other stuff should be set by the caller
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

; Source=(https://web.archive.org/web/20160619063203/http://www.ctyme.com/intr/rb-0621.htm)
drive_parameters:
	mov  dl, [BOOT_DISK]	
	mov  ah, 0x08
	int  0x13
	jc   disk_param_error

	;CF set on error
	;AH = status (07h) 
	;CF clear if successful
	;AH = 00h
	;AL = 00h on at least some BIOSes
	;BL = drive type (AT/PS2 floppies only) 
	;CH = low eight bits of maximum cylinder number
	;CL = maximum sector number (bits 5-0)-
	; -high two bits of maximum cylinder number (bits 7-6)
	;DH = maximum head number
	;DL = number of drives

	;mov  bh, cl
	and  cl, 0x3f
	;and  bh, 0xfc
    
	mov  [drv_params_entry], bl     ; Drive Type
	mov  [drv_params_entry+1], ch   ; Max. Cylinder
	mov  [drv_params_entry+2], cl   ; Max. Sector
	mov  [drv_params_entry+3], dh   ; Max. Head
	mov  [drv_params_entry+4], dl   ; N. Drives

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
	xor ebx, ebx ; Only the lower byte is relevant
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
