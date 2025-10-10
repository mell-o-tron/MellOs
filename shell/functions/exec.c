#ifdef VGA_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "file_system.h"

#define FDEF(name) void name(const char* s)

FDEF(exec){
    const char *t = s;

    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }
    
    if (t - s >= 128) {
        kprint("Arguments not recognized.\n");
        return;
    }
    
    char* exec_place = (char*)0x700000; // TODO: Noooo please allocate memory I want to exec more files 😭
    
    char* tmp = read_string_from_file((char*)t);
    
    for (int i = 0; i < 512; i++){
        exec_place[i] = tmp[i];
    }
    
    
    void (*src)() = (void*) exec_place;
    
    src();
}
 
