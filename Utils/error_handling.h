#pragma once
#include "Typedefs.h"
#include "assert.h"

#define fail(code) return (maybe_int){false, code}
#define succeed() return (maybe_int){true, 0}
#define just(code) (maybe_int){true, code}
#define failed(x) !x.is_some
#define succeeded(x) x.is_some

int unwrap (maybe_int x);

int halt_on_fail (maybe_void x);
int wat_on_fail (maybe_void x);
int wat_err_on_fail (maybe_void x);
int msg_on_fail (maybe_void x, const char * msg);
