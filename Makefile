## Compiler
CC=/usr/local/i386elfgcc/bin/i386-elf-gcc
## Linker
LD=/usr/local/i386elfgcc/bin/i386-elf-ld

SRC=$(shell pwd)
## Directory to write binaries to
BIN=./WeeBins
## Compiler Flags
FLAGS=-ffreestanding -m32 -g

## C++ source files
CPPSRC := $(shell find ./ -name "*.cpp")
## C++ target files
CPPTAR := $(patsubst %.cpp,%.o,$(CPPSRC))

## Assembly source files that must be compiled to ELF
ASMSRC := ./CPU/GDT/gdt_loader.asm ./Bootloader/basic_gdt.asm ./Kernel/kernel_entry.asm
## Assembly target files
ASMTAR := $(patsubst %.asm,%.o,$(ASMSRC))

## Files which must be linked first, if things break just bodge it together
LDPRIORITY := $(BIN)/Kernel/kernel_entry.o $(BIN)/Kernel/kernel.o $(BIN)/Drivers/Keyboard.o $(BIN)/CPU/Interrupts/irq.o $(BIN)/Drivers/VGA_Text.o

all: prebuild build run

prebuild:	## Prebuild instructions
	clear
	rm -rf $(BIN)
	mkdir $(BIN)

build: boot $(ASMTAR) $(CPPTAR)
	$(LD) -o $(BIN)/kernel.bin -Ttext 0x1000 $(LDPRIORITY) --start-group $(filter-out $(LDPRIORITY),$(shell find ./ -name "*.o" | xargs)) --end-group --oformat binary ## Pray this works
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
## qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy,  -m 128M
	qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy -hda disk.img -m 128M