CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

gdt_start:
gdt_null:
	dd 0x00
	dd 0x00
gdt_code:
	dw 0xFFFF
	dw 0x00
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00
gdt_data:
	dw 0xFFFF
	dw 0x00
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00
gdt_end:
gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start