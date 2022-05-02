## Compiler
CC=/usr/local/i386elfgcc/bin/i386-elf-gcc
## Linker
LD=/usr/local/i386elfgcc/bin/i386-elf-ld
SRC=$(shell pwd)
## Directory to write binaries to
BIN=WeeBins
## Compiler Flags
FLAGS=-ffreestanding -m32 -g

## C++ source files
CPPSRC := $(shell find ./ -name "*.cpp")
## C++ target files
CPPTAR := $(patsubst %.cpp,%.o,$(CPPSRC))

## Assembly files to ignore in ASMTAR - these need to be compiled to .bin or not at all
ASMBIN := ./Bootloader/boot.o ./Kernel/empty_end.o ./Bootloader/AvailableMemory.o ./Bootloader/EnterPM.o ./Bootloader/PrintDecimal.o ./Bootloader/PrintString.o ./Bootloader/PrintStringPM.o ./Bootloader/ReadFromDisk.o ./CPU/Interrupts/interrupt.o ./Kernel/IncBins.o 
ASMSRC := $(shell find ./ -name "*.asm")
ASMTAR := $(filter-out $(ASMBIN),$(patsubst %.asm,%.o,$(ASMSRC)))

all: prebuild build run

prebuild:	## Prebuild instructions
	clear
	rm -rf $(BIN)
	mkdir $(BIN)

build: boot $(ASMTAR) $(CPPTAR)
	$(LD) -o $(BIN)/kernel.bin -Ttext 0x1000 --start-group $(BIN)/Kernel/kernel_entry.o $(BIN)/Kernel/kernel.o $(BIN)/Drivers/VGA_Text.o $(BIN)/Drivers/port_io.o $(BIN)/Utils/Conversions.o $(BIN)/Memory/mem.o $(BIN)/CPU/Interrupts/idt.o $(BIN)/CPU/Interrupts/isr.o $(BIN)/CPU/Interrupts/irq.o $(BIN)/CPU/Timer/timer.o $(BIN)/Drivers/Keyboard.o $(BIN)/Misc/CmdMode.o $(BIN)/Utils/string.o $(BIN)/Misc/CodeMode.o $(BIN)/Utils/dataStructures.o $(BIN)/Shell/shell.o $(BIN)/Drivers/Floppy.o $(BIN)/Shell/shellFunctions.o $(BIN)/CPU/GDT/gdt_loader.o $(BIN)/CPU/GDT/gdt.o --end-group --oformat binary
	cat $(BIN)/boot.bin $(BIN)/kernel.bin > $(BIN)/short.bin
	cat $(BIN)/short.bin $(BIN)/empty_end.bin > os_image.bin

boot:
	nasm Bootloader/boot.asm -f bin -o $(BIN)/boot.bin -i Bootloader
	nasm Kernel/empty_end.asm -f bin -o $(BIN)/empty_end.bin

%.o: %.cpp
	mkdir -p $(BIN)/$(shell dirname $<)
	$(CC) $(FLAGS) -c $< -o $(BIN)/$(subst .cpp,.o,$<) $(addprefix -I ,$(shell dirname $(shell echo $(CPPSRC) | tr ' ' '\n' | sort -u | xargs)))

%.o : %.asm
	mkdir -p $(BIN)/$(shell dirname $<)
	nasm $< -f elf -o $(BIN)/$(subst .asm,.o,$<) $(addprefix -i ,$(shell dirname $(shell echo $(CPPSRC) | tr ' ' '\n' | sort -u | xargs)))

run:
	qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy,  -m 128M