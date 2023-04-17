#include "../CPU/Interrupts/idt.h"
#include "../Drivers/VGA_Text.h"

int syscall_stub (regs *r, int number){
    switch (number) {
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
    kprint("sys_write\n");
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
