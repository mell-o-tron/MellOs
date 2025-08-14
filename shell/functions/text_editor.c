#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../utils/typedefs.h"
#include "../../text_editor/text_editor.h"

#define FDEF(name) void name(const char* s)

FDEF(texted){
    const char* t = s;

    char filename [10];

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
        kprint("Filename too long.\n");
        return;
    }

    init_text_editor(filename);

}

