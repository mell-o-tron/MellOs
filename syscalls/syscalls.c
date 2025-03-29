#include "../cpu/interrupts/idt.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "../utils/conversions.h"
#include "../file_system/file_system.h"
#include "../utils/conversions.h"
#include "../memory/dynamic_mem.h"
#include "../shell/shell.h"
#include "../utils/string.h"
#include "syscalls.h"

int syscall_stub (regs *r){
    switch (r -> eax) {
        case 1:
            //sys_exit
            return sys_exit(r);
            
        case 2:
            //sys_fork
            return sys_fork(r);
        case 3:
            //sys_read
            return sys_read(r);
        case 4:
            //sys_write
            return sys_write(r);
        case 5:
            //sys_open
            return sys_open(r);
        case 6:
            //sys_close
            return sys_close(r);
    }
    
    return 0;
}


int sys_exit (regs *r){
     kprint("sys_exit\n");
    return -1;   
}

int sys_fork (regs *r){
    kprint("sys_fork\n");
    return -1;   
}

int sys_read (regs *r){
    kprint("sys_read\n");
    return -1;   
}

int sys_write (regs *r){
    uint32_t LBA = r -> ebx;
    char* msg = (char*) (r -> ecx);
    
    uint32_t len = r -> edx;
    
    // If the file descriptor is 1, we write to "stdout"
    // Which for now does not exist lol, we just print.
    if(LBA == 1){
        if(strlen(msg) > len) msg[len - 1] = 0;
        kprint(msg);
        return 0;
    }

    char* tmp = kmalloc((len / 512 + 1) * 512);
    
    for (uint32_t i = 0; i < (len / 512 + 1) * 512; i++) {
        if (i < len) 
            tmp[i] = msg[i];
        else
            tmp[i] = 0;
    }

    
    file_t * files = get_file_list(0xA0, 1, 1);
    
    for (uint32_t i = 0; i < 32; i++) {
        if (LBA == files[i].LBA) {
            add_filewrite_task(tmp, files[i].name, len);
            return 0;
        }
    }    
    
    return -1;   
}

int sys_open (regs *r){
    kprint("sys_open\n");
    return -1;   
}

int sys_close (regs *r){
    kprint("sys_close\n");
    return -1;   
}
