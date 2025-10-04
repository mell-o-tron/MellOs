#pragma once

#include "kernel.h"

#define assert(c) if(!(c)) { kpanic_message(#c); }
