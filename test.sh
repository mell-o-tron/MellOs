export PATH=$PATH:/usr/local/i386elfgcc/bin
i386-elf-gcc -ffreestanding -m32 -g -c "CPU/GDT/gdt.cpp" -o "A/gdt.o"
nasm "CPU/GDT/gdt_loader.asm" -f elf -o "A/ts.o"

i386-elf-ld -o "A/Aa.bin" -Ttext 0x1000 "A/ts.o" "A/gdt.o" --oformat binary