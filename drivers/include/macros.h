#pragma once
#include "kernel_stdio.h"



// DRIVER_NAME is set inside the driver

#define ERR(s) \
    kprintf("[" DRIVER_NAME " ERROR] %s\n", s);

#define ERRF(f, args...) \
    kvsnprintf(buf, sizeof(buf), f, ##args); \
    kprintf("%s %s" , "[" DRIVER_NAME " ERROR] ", buf);

#define ERRDI(desc, i) \
    kprintf("[" DRIVER_NAME " ERROR] %s %i\n", desc, i);

#define ERRI(i) kprintf("[" DRIVER_NAME " ERROR] %i\n", i);

#define INFO(s) \
    kprintf("[" DRIVER_NAME " INFO] %s\n", s);

#define INFOF(f, args...) \
    kvsnprintf(buf, sizeof(buf), f, ##args); \
    kprintf("%s %s" , "[" DRIVER_NAME " INFO] ", buf);

#define INFODI(desc, i) \
    kprintf("[" DRIVER_NAME " INFO] %s %i\n", desc, i);

#define INFOI(i) \
    kprintf("[" DRIVER_NAME " INFO] %i\n", i);
