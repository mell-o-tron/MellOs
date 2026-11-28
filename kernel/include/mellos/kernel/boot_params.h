#pragma once

#include "stdbool.h"
#include "stddef.h"
#define BOOT_PARAMS_LENGTH 256
__attribute__((section(".low.bss"))) static char boot_cmdline[BOOT_PARAMS_LENGTH];
static char boot_params_storage[BOOT_PARAMS_LENGTH];


enum boot_parameter_format {
    PARAM_TYPE_STRING, PARAM_TYPE_INT32, PARAM_TYPE_BOOL
};

typedef struct boot_parameter {
    bool last;
    void* value;
    enum boot_parameter_format format;
    char* keyname;
    char* default_value;
} boot_parameter_t;

static boot_parameter_t boot_params[] = {
    {false,NULL,PARAM_TYPE_BOOL,"debug","false"},
    {false,NULL,PARAM_TYPE_STRING,"root","/dev/ram0p1"},
    {true,NULL,PARAM_TYPE_STRING,"rootfstype","fat12"}
};

/**
 * get a kernel command line argument
 * example: `root=/dev/hd0p1 rootfstype=fat12 debug=true`
 * @param key key
 * @param default_value value if no value is specified
 * @return the value defined in cmdline or default_value if not found
 */
char* cmdline_get(const char* key, char* default_value);

void* param_get_address(const char* key);

int cache_boot_params();