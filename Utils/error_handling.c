#include "Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"
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
        kprint(toString(x.val, 10));
        for(;;);
    }
    return unwrap(x);
}

int msg_on_fail (maybe_void x, const char * msg){
    if (!x.is_some){
        kprint(msg);
        kprint(" - error code: ");
        kprint(toString(x.val, 10));
        for(;;);
    }

    return unwrap(x);
}



// TODO ADD:
// repeat_on_fail
// repeat_until_not_fail
// some advanced nullcheck(?)
