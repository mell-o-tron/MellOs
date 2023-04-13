## Bootloader

The bootloader is the chronologically first interface of our operating system with the machine.

The goal of the bootloader is to load the kernel into memory. In addition to this, it will have to set the stage for it, and provide it with some information, e.g. the memory map.

### Memory detection

There are several ways to detect memory in the bootloader. We're using two different functions, namely `memory_detection` and `upper_mem_map`.

- The first uses `INT 0x15 EAX=0xe820` and stores the result at memory location `0x5000`
- The second uses `INT 0x15 AX=0xe801` and stores the result at memory location `0x5100`:

```nasm
upper_mem_map:
    mov ax, 0xe801
    int 0x15
    jc mem_error
    mov [mmap_1_entry], ax          ; extended 1 (up to 15MB between 1MB and 16MB)
    mov [mmap_1_entry + 2], bx      ; extended 2 (number of contiguous 64KB blocks
                                    ;             between 16MB and 4GB)
    ret
```

The result is then read by the kernel into a `mem_t` data structure:

```C
typedef struct {
    uint16_t lower_size;
    uint16_t upper_size;
} mem_t;
```

Since the second one appears to be more precise at the moment, we're using the `extended_1` result to set a limit for the linear allocation in lower memory. The `extended_2` result will be used to set a limit for the creation of pages (and will always need to be converted to virtual memory).

The values at memory location `0x5000` and `0x5100` can be easily modified at the moment, and will have to be protected somehow (e.g. copied to a read only page).

Another way to pass information to the kernel is to write some value into `bx` near this comment:

```
; WHATEVER IS PUT INTO BX HERE WILL BE WRITTEN INTO THE MEMSIZE VARIABLE!
; Can be adapted to pass any information to the kernel without storing it in memory
```

### Read the kernel code from disk

The `disk_read` function is called as follows

```nasm
	;read 50 sectors at kernel location
	mov bx, KERNEL_LOCATION
	mov dh, 50
	call disk_read
```

This function uses `INT 0x13` to read from disk. The kernel location is currently `0x7ef0`

### Protected Mode & the GDT

The GDT (defined in `gdt.asm`) is loaded, and a far jump to `CODE_SEG:_main32` is performed.

```nasm
_main32:
	;set up segment registers again
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;set up the stack
	mov ebp, 0x90000
	mov esp, ebp

	;enable the A20 line
	in al, 0x92
	or al, 0x02
	out 0x92, al

	;jump to kernel location
	jmp KERNEL_LOCATION
```

As we're working on paging and we now have a decent memory map, the stack will have to be initialized to a less random value. After this, the kernel is loaded. See the kernel docs.
