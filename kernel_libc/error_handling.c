#include "stdio.h"
#include "stddef.h"
#include "assert.h"
#include "kernel_stdio.h"

int unwrap (maybe_int x){
    kassert(x.is_some);

    return x.val;
}

int halt_on_fail (maybe_void x){
    if (!x.is_some)
        for(;;) {}

    return unwrap(x);
}

int wat_on_fail (maybe_void x){
    if (!x.is_some){
        kfprintf(kstderr, "wat..?");
        for(;;);
    }
    return unwrap(x);
}

int wat_err_on_fail (maybe_void x){
    if (!x.is_some){
        kfprintf(kstderr, "wat..? - error code: ");
        kfprintf(kstderr, "%i", x.val);
        for(;;);
    }
    return unwrap(x);
}

int msg_on_fail (maybe_void x, const char *msg){
    if (!x.is_some){
        kfprintf(kstderr, msg);
        kfprintf(kstderr, " - error code: %i", x.val);
        for(;;);
    }
    return unwrap(x);
}



// TODO ADD:
// repeat_on_fail
// repeat_until_not_fail
// some advanced nullcheck(?)
