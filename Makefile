.PHONY: all debug prebuild boot run clean

VGA ?= VGA_VESA
HRES ?= 1920
VRES ?= 1080
BPP ?= 32

## Compiler
CC=/usr/local/i386elfgcc/bin/i386-elf-gcc

OBJCP=/usr/local/i386elfgcc/bin/i386-elf-objcopy
## Linker
LD=/usr/local/i386elfgcc/bin/i386-elf-ld

SRC=$(shell pwd)
## Directory to write binaries to
BIN=./wee_bins
## Compiler Flags
FLAGS=-ffreestanding -m32 -g -D$(VGA) -DHRES=$(HRES) -DVRES=$(VRES) -DBPP=$(BPP) -O3
## NASM Flags
NASMFLAGS=-D$(VGA) -DHRES=$(HRES) -DVRES=$(VRES) -DBPP=$(BPP)

## C source files
CSRC := $(shell find ./ -name "*.c")
## C target files
CTAR := $(patsubst %.c,%.o,$(CSRC))

## Assembly source files that must be compiled to ELF
ASMSRC := ./cpu/gdt/gdt_loader.asm ./kernel/kernel_entry.asm ./processes/processes_asm.asm
## Assembly target files
ASMTAR := $(patsubst %.asm,%.o,$(ASMSRC))

all:
	$(MAKE) prebuild
	$(MAKE) build

debug:
	$(MAKE) prebuild
	$(MAKE) build
	$(OBJCP) --only-keep-debug $(BIN)/kernel.elf $(BIN)/kernel.sym

	qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy -hda test_disk.img -m 128M -s -S -vga std &

prebuild: clean	## Prebuild instructions
	clear
	mkdir $(BIN)

build: $(ASMTAR) $(CTAR) $(FONTTAR)
	$(LD) -o $(BIN)/kernel.elf -Tkernel/kernel.ld $(shell find ./ -name "*.o" | xargs)
	cp $(BIN)/kernel.elf iso/boot/
	grub-mkrescue -o mellos.iso iso

%.o: %.c
	mkdir -p $(BIN)/$(shell dirname $<)
	$(CC) $(FLAGS) -c $< -o $(BIN)/$(subst .c,.o,$<) $(addprefix -I ,$(shell dirname $(shell echo $(CSRC) | tr ' ' '\n' | sort -u | xargs)))

%.o : %.asm
	mkdir -p $(BIN)/$(shell dirname $<)
	nasm $< -f elf $(NASMFLAGS) -o $(BIN)/$(subst .asm,.o,$<) $(addprefix -i ,$(shell dirname $(shell echo $(CSRC) | tr ' ' '\n' | sort -u | xargs)))

run: all
## qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy,  -m 128M
## qemu-system-x86_64 -d cpu_reset -drive format=raw,file=os_image.bin,index=0,if=disk -hda test_disk.img -m 128M
## qemu-system-x86_64 -d cpu_reset -drive format=raw,file=os_image.bin,index=0,if=floppy -m 128M
	qemu-system-i386 -cdrom mellos.iso -no-reboot -serial file:serial.log -monitor stdio -d int,cpu_reset -D qemu_debug.log -hda test_disk.img


clean:
	rm -rf $(BIN)
	rm -f os_image.bin
