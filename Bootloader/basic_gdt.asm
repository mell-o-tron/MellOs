GDT_start: 				; to define the Global Descriptor Table we define bytes, words and double words
                        ; to write them "raw" in the bin file (like we did in the disk reading example, or to define the
                        ; end of the bootsector)

GDT_null:				; mandatory NULL descriptor
	dd 0x0
	dd 0x0

GDT_code:				; code segment descriptor
	dw 0xffff			; limit, bits 0-15 (completed at line 14)
	dw 0x0				; Base, bits 0-15
	db 0x0				; Base, bits 16-23 (completed at line 15)
	db 0b10011010		; Flags (look at paper for meaning)
	db 0b11001111		; Flags, Limit bits 16-19
	db 0x0				; Base, bits 24-31

GDT_data:				; data segment descriptor
	dw 0xffff			; limit, bits 0-15
	dw 0x0				; Base, bits 0-15
	db 0x0				; Base, bits 16-23
	db 0b10010010		; Flags (look at paper for meaning) 		DATA FLAGS ARE DIFFERENT IN DATA SEGMENT
	db 0b11001111		; Flags, Limit bits 16-19
	db 0x0				; Base, bits 24-31

GDT_end:

GDT_descriptor:
	dw GDT_end - GDT_start - 1	    ; Size of GDT - 1
	dd GDT_start			        ; start of GDT
	

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start
