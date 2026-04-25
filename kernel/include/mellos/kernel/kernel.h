#pragma once
#include "stdint.h"

#ifndef asmlinkage
#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define asmlinkage __attribute__((regparm(0)))
#else
#define asmlinkage extern
#endif
#endif

__attribute__((section(".low.bss"))) static char boot_cmdline[256];

/**
 * get a kernel command line argument
 * example: `root=/dev/hd0p1 rootfstype=fat12 debug=1`
 * the maximum allowed length for a value is 128
 * @param key key
 * @param default_value value if no value is specified
 * @return the value defined in cmdline or default_value if not found
 */
const char* cmdline_get(const char* key, const char* default_value);

extern void kpanic(struct regs* r);

/**
 * Prefix the message with the name of the function that the panic was called from, example:
 * "example_function: x is NULL"
 * so that it is easy to grep "example_function("
 * @param msg the message to provide in the panic screen
 */
void kpanic_message(const char* msg);
_Noreturn void higher_half_main(uintptr_t multiboot_tags_addr);