#include "./../Utils/Typedefs.h" 
#include "./../Drivers/port_io.h" 

unsigned short get_CMOS_memory_size(){
    unsigned short total;
    unsigned char lowmem, highmem;
 
    outb(0x70, 0x30);
    lowmem = inb(0x71);
    outb(0x70, 0x31);
    highmem = inb(0x71);
 
    total = lowmem | highmem << 8;
    return total;
}
