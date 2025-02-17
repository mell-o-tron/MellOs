#include "typedefs.h"
#include "../drivers/vga_text.h"
#include "../utils/conversions.h"
#include <assert.h>

int unwrap (maybe_int x){
    assert (x.is_some);

    return x.val;
}

int halt_on_fail (maybe_void x){
    if (!x.is_some)
        for(;;);
        
    return unwrap(x);
}

int wat_on_fail (maybe_void x){
    if (!x.is_some){
        kprint("wat..?");
        for(;;);
    }
    return unwrap(x);
}

int wat_err_on_fail (maybe_void x){
    if (!x.is_some){
        kprint("wat..? - error code: ");
        char tostr_buffer[20];
        kprint(tostring(x.val, 10, tostr_buffer));
        for(;;);
    }
    return unwrap(x);
}

int msg_on_fail (maybe_void x, const char * msg){
    if (!x.is_some){
        kprint(msg);
        kprint(" - error code: ");
        char tostr_buffer[20];
        kprint(tostring(x.val, 10, tostr_buffer));
        for(;;);
    }
    return unwrap(x);
}



// TODO ADD:
// repeat_on_fail
// repeat_until_not_fail
// some advanced nullcheck(?)
