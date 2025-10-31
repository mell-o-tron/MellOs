[bits 32]
section .low.text
global loadPageDirectory
loadPageDirectory:
    push ebp
    mov ebp, esp
    mov eax, [esp+8]
    mov cr3, eax
    mov esp, ebp
    pop ebp
    ret

global enablePaging
enablePaging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret
section .text
global disablePaging
disablePaging:
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax
    ret
