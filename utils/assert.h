#pragma once

#include "kernel.h"

#define assert(c) if(!(c)) { kpanic_message(#c); }
#define assert_msg(c, msg) if(!(c)) { kpanic_message(msg); }
