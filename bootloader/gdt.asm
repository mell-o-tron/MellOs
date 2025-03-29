CODE_SEG equ gdt_code - gdt_start ; 0x08
DATA_SEG equ gdt_data - gdt_start ; 0x10

gdt_start:
gdt_null:
	dd 0x00000000
	dd 0x00000000
gdt_code:
	dw 0xFFFF		; limit low
	dw 0x0000		; base low
	db 0x00			; base middle
	db 10011011b	; access: 1Present, 2privileged, 1code, 1executable, 1executable from privileged, 1readable, 1accessed
	db 11001111b	; granularity: 4KiB, 32bit, not64-bit, reserved0, 4limit high
	db 0x00			; base high
gdt_data:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 10010011b	; 1Present, 2privileged, 1data, 1notexec, 1grow up, 1writable, 1accessed
	db 11001111b
	db 0x00
gdt_end:
gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start