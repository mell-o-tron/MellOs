#include "stdio.h"
#include "include/stddef.h"
#include "assert.h"

int unwrap (maybe_int x){
    assert(x.is_some);

    return x.val;
}

int halt_on_fail (maybe_void x){
    if (!x.is_some)
        for(;;) {}

    return unwrap(x);
}

int wat_on_fail (maybe_void x){
    if (!x.is_some){
        fprintf(stderr, "wat..?");
        for(;;);
    }
    return unwrap(x);
}

int wat_err_on_fail (maybe_void x){
    if (!x.is_some){
        fprintf(stderr, "wat..? - error code: ");
        fprintf(stderr, "%i", x.val);
        for(;;);
    }
    return unwrap(x);
}

int msg_on_fail (maybe_void x, const char *msg){
    if (!x.is_some){
        fprintf(stderr, msg);
        fprintf(stderr, " - error code: %i", x.val);
        for(;;);
    }
    return unwrap(x);
}



// TODO ADD:
// repeat_on_fail
// repeat_until_not_fail
// some advanced nullcheck(?)
