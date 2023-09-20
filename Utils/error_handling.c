#include "Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"

int halt_on_fail (maybe_void x){
    if (x < 0)
        for(;;);
        
    return x;
}

int wat_on_fail (maybe_void x){
    if (x < 0){
        kprint("wat..?");
        for(;;);
    }
    return x;
}

int wat_err_on_fail (maybe_void x){
    if (x < 0){
        kprint("wat..? - error code: ");
        kprint(toString(x, 10));
        for(;;);
    }
    return x;
}

int msg_on_fail (maybe_void x, const char * msg){
    if (x < 0){
        kprint(msg);
        kprint(" - error code: ");
        kprint(toString(x, 10));
        for(;;);
    }
    return x;
}

// TODO ADD:
// repeat_on_fail
// repeat_until_not_fail
// some advanced nullcheck(?)
