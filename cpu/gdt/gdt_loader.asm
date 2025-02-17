[bits 32]

global gdt_load
gdt_load:
    
    ; make new call frame
    push ebp            
    mov ebp, esp         
    
    ; load gdt
    mov eax, [ebp + 8]
    lgdt [eax]

    ; reload code segment
    mov eax, [ebp + 12]
    push eax
    push .reload_cs
    retf

.reload_cs:

    ; reload data segments
    mov ax, [ebp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret