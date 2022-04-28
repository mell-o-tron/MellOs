export PATH=$PATH:/usr/local/i386elfgcc/bin
echo Boot:
nasm "Bootloader/boot.asm" -f bin -o "WeeBins/boot.bin" -i Bootloader
nasm "Kernel/empty_end.asm" -f bin -o "WeeBins/empty_end.bin"

echo Kernel:
i386-elf-gcc -ffreestanding -m32 -g -c "Kernel/kernel.cpp" -o "WeeBins/kernel.o" -I Kernel
nasm "Kernel/kernel_entry.asm" -f elf -o "WeeBins/kernel_entry.o" -i Kernel

echo Drivers + Utils:
i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/port_io.cpp" -o "WeeBins/Drivers/port_io.o" -I Drivers
i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/VGA_Text.cpp" -o "WeeBins/Drivers/VGA_Text.o" -I Drivers
i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/Keyboard.cpp" -o "WeeBins/Drivers/Keyboard.o" -I Drivers
i386-elf-gcc -ffreestanding -m32 -g -c "Drivers/Floppy.cpp" -o "WeeBins/Drivers/Floppy.o" -I Drivers

i386-elf-gcc -ffreestanding -m32 -g -c "Utils/Conversions.cpp" -o "WeeBins/Utils/Conversions.o" -I Utils
i386-elf-gcc -ffreestanding -m32 -g -c "Utils/string.cpp" -o "WeeBins/Utils/string.o" -I Utils
i386-elf-gcc -ffreestanding -m32 -g -c "Utils/dataStructures.cpp" -o "WeeBins/Utils/dataStructures.o" -I Utils
echo CPU:
echo ----GDT:
i386-elf-gcc -ffreestanding -m32 -g -c "CPU/GDT/gdt.cpp" -o "WeeBins/CPU/gdt.o" -I GDT
nasm "CPU/GDT/gdt_loader.asm" -f elf -o "WeeBins/CPU/gdt_loader.o"
echo ----Interrupts:
i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Interrupts/idt.cpp" -o "WeeBins/CPU/idt.o"
i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Interrupts/isr.cpp" -o "WeeBins/CPU/isr.o" -I Interrupts
i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Interrupts/irq.cpp" -o "WeeBins/CPU/irq.o" -I Interrupts
echo ----Timer:
i386-elf-gcc -ffreestanding -m32 -g -c "CPU/Timer/timer.cpp" -o "WeeBins/CPU/timer.o" -I Timer

echo MEMORY:
i386-elf-gcc -ffreestanding -m32 -g -c "Memory/mem.cpp" -o "WeeBins/Memory/mem.o"

echo MISC:
i386-elf-gcc -ffreestanding -m32 -g -c "Misc/CmdMode.cpp" -o "WeeBins/Misc/CmdMode.o" -I Misc
i386-elf-gcc -ffreestanding -m32 -g -c "Misc/CodeMode.cpp" -o "WeeBins/Misc/CodeMode.o" -I Misc

echo SHELL:
i386-elf-gcc -ffreestanding -m32 -g -c "Shell/shell.cpp" -o "WeeBins/Shell/shell.o" -I Shell
i386-elf-gcc -ffreestanding -m32 -g -c "Shell/shellFunctions.cpp" -o "WeeBins/Shell/shellFunctions.o" -I Shell

echo Linking:
i386-elf-ld -o "WeeBins/kernel.bin" -Ttext 0x1000 "WeeBins/kernel_entry.o" "WeeBins/kernel.o" "WeeBins/Drivers/VGA_Text.o" "WeeBins/Drivers/port_io.o" "WeeBins/Utils/Conversions.o" "WeeBins/Memory/mem.o" "WeeBins/CPU/idt.o" "WeeBins/CPU/isr.o" "WeeBins/CPU/irq.o" "WeeBins/CPU/timer.o" "WeeBins/Drivers/Keyboard.o" "WeeBins/Misc/CmdMode.o" "WeeBins/Utils/string.o" "WeeBins/Misc/CodeMode.o" "WeeBins/Utils/dataStructures.o" "WeeBins/Shell/shell.o" "WeeBins/Drivers/Floppy.o" "WeeBins/Shell/shellFunctions.o" "WeeBins/CPU/gdt_loader.o" "WeeBins/CPU/gdt.o" --oformat binary


cat "WeeBins/boot.bin" "WeeBins/kernel.bin" > "WeeBins/short.bin"
cat "WeeBins/short.bin" "WeeBins/empty_end.bin" > os_image.bin



#qemu-system-x86_64 -s -S -drive format=raw,file=os_image.bin,index=0,if=floppy		# GDB
qemu-system-x86_64 -drive format=raw,file=os_image.bin,index=0,if=floppy,  -m 128M			# RUN EMULATOR