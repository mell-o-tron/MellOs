#include "../../file_system/file_system.h"
#include "../../drivers/vga_text.h"

#define FDEF(name) void name(const char* s)

FDEF(ls){
    file_t * files = get_file_list(0xA0, 1, 1);
    print_files(files, 32);
}

FDEF(newfile){
    const char* t = s;

    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }

    if(t - s >= 128){
        kprint("Arguments not recognized.\n");
        return;
    }

    new_file((char*)t);
}

FDEF(write_file){
    const char* t = s;

    char filename [10];
    const char* str;

    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }

    if(t - s >= 128){
        kprint("Arguments not recognized.\n");
        return;
    }

    uint32_t i = 0;
    while (*t != ' ' && *t != '\t' && *t != 0 && *t > 33 && *t < 126 && i < 10) {
        filename[i] = *t;
        t++;
        i++;
    }

    if(i < 10)
        filename[i] = 0;

    if(*t != ' ' && *t != '\t' && *t > 33 && *t < 126){
        kprint("Either filename too long or no second argument.\n");
        return;
    }

    // skip spaces again
    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }

    if(t - s >= 128){
        kprint("Enter a second argument.\n");
        return;
    }

    str = t;

    // kprint("you entered the following:\n");
    // filename[9] = 0;
    // kprint(filename);
    // kprint("\n");
    // kprint(str);
    // kprint("\n");

    write_string_to_file((char*)str, filename);
}

FDEF(read_file){
    const char* t = s;
    while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
        t++;
    }

    char* res = read_string_from_file((char*)t);

    if(res != NULL)
        kprint(res);
}
