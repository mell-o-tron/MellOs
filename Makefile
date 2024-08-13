## Compiler
CC=/usr/local/i386elfgcc/bin/i386-elf-gcc

OBJCP=/usr/local/i386elfgcc/bin/i386-elf-objcopy
## Linker
LD=/usr/local/i386elfgcc/bin/i386-elf-ld

SRC=$(shell pwd)
## Directory to write binaries to
BIN=./WeeBins
## Compiler Flags
FLAGS=-ffreestanding -m32 -g 

## C source files
CSRC := $(shell find ./ -name "*.c")
## C target files
CTAR := $(patsubst %.c,%.o,$(CSRC))

## Assembly source files that must be compiled to ELF
ASMSRC := ./CPU/GDT/gdt_loader.asm ./Bootloader/gdt.asm ./Kernel/kernel_entry.asm
## Assembly target files
ASMTAR := $(patsubst %.asm,%.o,$(ASMSRC))

all: prebuild build

debug: prebuild build
	$(OBJCP) --only-keep-debug $(BIN)/kernel.elf $(BIN)/kernel.sym

	qemu-system-x86_64 -drive format=raw,file=osimage_formated.bin,index=0,if=floppy -hda disk.img -m 128M -s -S &

prebuild:	## Prebuild instructions
	clear
	rm -rf $(BIN)
	mkdir $(BIN)

build: boot $(ASMTAR) $(CTAR)
	$(LD) -o $(BIN)/kernel.elf -TKernel/kernel.ld $(shell find ./ -name "*.o" | xargs)
	$(OBJCP) -O binary $(BIN)/kernel.elf $(BIN)/kernel.bin
	cat $(BIN)/boot.bin $(BIN)/stage_2.bin > $(BIN)/both_boot.bin
	cat $(BIN)/both_boot.bin $(BIN)/kernel.bin > $(BIN)/short.bin
	cat $(BIN)/short.bin $(BIN)/empty_end.bin > os_image.bin
	dd if=/dev/zero of=osimage_formated.bin bs=512 count=2880 >/dev/null
	dd if=os_image.bin of=osimage_formated.bin conv=notrunc >/dev/null

boot:
	nasm Bootloader/boot.asm -f bin -o $(BIN)/boot.bin -i Bootloader
	nasm Bootloader/stage_2.asm -f bin -o $(BIN)/stage_2.bin -i Bootloader
	nasm Kernel/empty_end.asm -f bin -o $(BIN)/empty_end.bin

%.o: %.c
	mkdir -p $(BIN)/$(shell dirname $<)
	$(CC) $(FLAGS) -c $< -o $(BIN)/$(subst .c,.o,$<) $(addprefix -I ,$(shell dirname $(shell echo $(CSRC) | tr ' ' '\n' | sort -u | xargs)))

%.o : %.asm
	mkdir -p $(BIN)/$(shell dirname $<)
	nasm $< -f elf -o $(BIN)/$(subst .asm,.o,$<) $(addprefix -i ,$(shell dirname $(shell echo $(CSRC) | tr ' ' '\n' | sort -u | xargs)))

run: prebuild build
## qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy,  -m 128M
	qemu-system-x86_64 -d cpu_reset -drive format=raw,file=osimage_formated.bin,index=0,if=floppy -drive format=raw,file=test_disk.img, -m 128M
