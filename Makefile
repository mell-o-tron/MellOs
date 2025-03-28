.PHONY: all debug prebuild boot run clean

## Compiler
CC=/usr/local/i386elfgcc/bin/i386-elf-gcc

OBJCP=/usr/local/i386elfgcc/bin/i386-elf-objcopy
## Linker
LD=/usr/local/i386elfgcc/bin/i386-elf-ld

SRC=$(shell pwd)
## Directory to write binaries to
BIN=./wee_bins
## Compiler Flags
FLAGS=-ffreestanding -m32 -g 

## C source files
CSRC := $(shell find ./ -name "*.c")
## C target files
CTAR := $(patsubst %.c,%.o,$(CSRC))

## Assembly source files that must be compiled to ELF
ASMSRC := ./cpu/gdt/gdt_loader.asm ./bootloader/gdt.asm ./kernel/kernel_entry.asm ./processes/processes_asm.asm
## Assembly target files
ASMTAR := $(patsubst %.asm,%.o,$(ASMSRC))

all:
	$(MAKE) prebuild
	$(MAKE) build

debug:
	$(MAKE) prebuild
	$(MAKE) build
	$(OBJCP) --only-keep-debug $(BIN)/kernel.elf $(BIN)/kernel.sym

	qemu-system-x86_64 -drive format=raw,file=osimage_formated.bin,index=0,if=floppy -hda disk.img -m 128M -s -S &

prebuild: clean	## Prebuild instructions
	clear
	mkdir $(BIN)

build: boot $(ASMTAR) $(CTAR)
	$(LD) -o $(BIN)/kernel.elf -Tkernel/kernel.ld $(shell find ./ -name "*.o" | xargs)
	$(OBJCP) -O binary $(BIN)/kernel.elf $(BIN)/kernel.bin
	cat $(BIN)/boot.bin $(BIN)/stage_2.bin > $(BIN)/both_boot.bin
	cat $(BIN)/both_boot.bin $(BIN)/kernel.bin > $(BIN)/short.bin
	cat $(BIN)/short.bin $(BIN)/empty_end.bin > os_image.bin
	dd if=/dev/zero of=osimage_formated.bin bs=512 count=2880 >/dev/null
	dd if=os_image.bin of=osimage_formated.bin conv=notrunc >/dev/null

boot:
	nasm bootloader/boot.asm -f bin -o $(BIN)/boot.bin -i bootloader
	nasm bootloader/stage_2.asm -f bin -o $(BIN)/stage_2.bin -i bootloader
	nasm kernel/empty_end.asm -f bin -o $(BIN)/empty_end.bin

%.o: %.c
	mkdir -p $(BIN)/$(shell dirname $<)
	$(CC) $(FLAGS) -c $< -o $(BIN)/$(subst .c,.o,$<) $(addprefix -I ,$(shell dirname $(shell echo $(CSRC) | tr ' ' '\n' | sort -u | xargs)))

%.o : %.asm
	mkdir -p $(BIN)/$(shell dirname $<)
	nasm $< -f elf -o $(BIN)/$(subst .asm,.o,$<) $(addprefix -i ,$(shell dirname $(shell echo $(CSRC) | tr ' ' '\n' | sort -u | xargs)))

run: all
## qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy,  -m 128M
	qemu-system-x86_64 -d cpu_reset -drive format=raw,file=osimage_formated.bin,index=0,if=floppy -hda test_disk.img -m 128M

clean:
	rm -rf $(BIN)
	rm -f os_image.bin
	rm -f osimage_formated.bin