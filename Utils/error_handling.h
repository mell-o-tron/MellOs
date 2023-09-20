#pragma once
#include "Typedefs.h"
#define fail(code) if (code > 0) return (-code); else return code
#define succeed return 0
#define failed(x) x < 0
#define succeeded(x) x >= 0

int halt_on_fail (maybe_void x);
int wat_on_fail (maybe_void x);
int wat_err_on_fail (maybe_void x);
int msg_on_fail (maybe_void x, const char * msg);
