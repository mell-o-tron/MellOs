 
# definizione del compilatore e dei flag di compilazione
# che vengono usate dalle regole implicite
CC=gcc
CFLAGS=-g -Wall -O -std=c99
LDLIBS=-lm


export PATH := $(PATH):/usr/local/i386elfgcc/bin

# se si scrive solo make di default compila main 
all: os_image.bin 

	

	
os_image.bin: WeeBins/short.bin WeeBins/empty_end.bin
	cat "WeeBins/short.bin" "WeeBins/empty_end.bin" > os_image.bin

WeeBins/short.bin: WeeBins/boot.bin WeeBins/kernel.bin
	cat "WeeBins/boot.bin" "WeeBins/kernel.bin" > "WeeBins/short.bin"

WeeBins/kernel.bin: WeeBins/kernel_entry.o WeeBins/kernel.o WeeBins/Drivers/VGA_Text.o WeeBins/Drivers/port_io.o WeeBins/Utils/Conversions.o WeeBins/Memory/mem.o WeeBins/CPU/idt.o WeeBins/CPU/isr.o WeeBins/CPU/irq.o WeeBins/CPU/timer.o WeeBins/Drivers/Keyboard.o WeeBins/Misc/CmdMode.o WeeBins/Utils/string.o WeeBins/Misc/CodeMode.o WeeBins/Utils/dataStructures.o WeeBins/Shell/shell.o WeeBins/Drivers/Floppy.o WeeBins/Shell/shellFunctions.o WeeBins/CPU/gdt_loader.o WeeBins/CPU/gdt.o
	i386-elf-ld -o "WeeBins/kernel.bin" -Ttext 0x1000 "WeeBins/kernel_entry.o" "WeeBins/kernel.o" "WeeBins/Drivers/VGA_Text.o" "WeeBins/Drivers/port_io.o" "WeeBins/Utils/Conversions.o" "WeeBins/Memory/mem.o" "WeeBins/CPU/idt.o" "WeeBins/CPU/isr.o" "WeeBins/CPU/irq.o" "WeeBins/CPU/timer.o" "WeeBins/Drivers/Keyboard.o" "WeeBins/Misc/CmdMode.o" "WeeBins/Utils/string.o" "WeeBins/Misc/CodeMode.o" "WeeBins/Utils/dataStructures.o" "WeeBins/Shell/shell.o" "WeeBins/Drivers/Floppy.o" "WeeBins/Shell/shellFunctions.o" "WeeBins/CPU/gdt_loader.o" "WeeBins/CPU/gdt.o" --oformat binary


#BOOT
WeeBins/boot.bin: Bootloader/boot.asm
	nasm "Bootloader/boot.asm" -f bin -o "WeeBins/boot.bin" -i Bootloader
WeeBins/empty_end.bin: Kernel/empty_end.asm
	nasm "Kernel/empty_end.asm" -f bin -o "WeeBins/empty_end.bin"

#KERNEL

WeeBins/kernel.o: Kernel/kernel.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Kernel/kernel.cpp" -o "WeeBins/kernel.o" -I Kernel
WeeBins/kernel_entry.o: Kernel/kernel_entry.asm
	nasm "Kernel/kernel_entry.asm" -f elf -o "WeeBins/kernel_entry.o" -i Kernel

#DRIVERS + UTILS
"WeeBins/Drivers/port_io.o": "Drivers/port_io.cpp" 
	i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/port_io.cpp" -o "WeeBins/Drivers/port_io.o" -I Drivers

"WeeBins/Drivers/VGA_Text.o": "Drivers/VGA_Text.cpp" 
	i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/VGA_Text.cpp" -o "WeeBins/Drivers/VGA_Text.o" -I Drivers

WeeBins/Drivers/Keyboard.o: Drivers/Keyboard.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/Keyboard.cpp" -o "WeeBins/Drivers/Keyboard.o" -I Drivers

WeeBins/Drivers/Floppy.o: Drivers/Floppy.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/Floppy.cpp" -o "WeeBins/Drivers/Floppy.o" -I Drivers
WeeBins/Utils/Conversions.o: Utils/Conversions.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Utils/Conversions.cpp" -o "WeeBins/Utils/Conversions.o" -I Utils

WeeBins/Utils/string.o: Utils/string.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Utils/string.cpp" -o "WeeBins/Utils/string.o" -I Utils
WeeBins/Utils/dataStructures.o: Utils/dataStructures.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Utils/dataStructures.cpp" -o "WeeBins/Utils/dataStructures.o" -I Utils


# CPU
WeeBins/CPU/gdt.o: CPU/GDT/gdt.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "CPU/GDT/gdt.cpp" -o "WeeBins/CPU/gdt.o" -I GDT
	
WeeBins/CPU/gdt_loader.o: CPU/GDT/gdt_loader.asm
	nasm "CPU/GDT/gdt_loader.asm" -f elf -o "WeeBins/CPU/gdt_loader.o"

WeeBins/CPU/idt.o: CPU/Interrupts/idt.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Interrupts/idt.cpp" -o "WeeBins/CPU/idt.o"

WeeBins/CPU/isr.o: CPU/Interrupts/isr.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Interrupts/isr.cpp" -o "WeeBins/CPU/isr.o" -I Interrupts

WeeBins/CPU/irq.o: CPU/Interrupts/irq.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Interrupts/irq.cpp" -o "WeeBins/CPU/irq.o" -I Interrupts

WeeBins/CPU/timer.o: CPU/Timer/timer.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Timer/timer.cpp" -o "WeeBins/CPU/timer.o" -I Timer

#MEMORY
WeeBins/Memory/mem.o: Memory/mem.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Memory/mem.cpp" -o "WeeBins/Memory/mem.o"



#MISC
WeeBins/Misc/CmdMode.o: Misc/CmdMode.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Misc/CmdMode.cpp" -o "WeeBins/Misc/CmdMode.o" -I Misc

WeeBins/Misc/CodeMode.o: Misc/CodeMode.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Misc/CodeMode.cpp" -o "WeeBins/Misc/CodeMode.o" -I Misc

# SHELL
WeeBins/Shell/shellFunctions.o: Shell/shellFunctions.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Shell/shellFunctions.cpp" -o "WeeBins/Shell/shellFunctions.o" -I Shell
	
WeeBins/Shell/shell.o: Shell/shell.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "Shell/shell.cpp" -o "WeeBins/Shell/shell.o" -I Shell
