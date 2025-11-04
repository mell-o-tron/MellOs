#include "mellos/kernel/memory_mapper.h"

#include "stdio.h"

#define FDEF(name) void name(const char* s)

FDEF(meminfo){
	// printf has the [KERNEL] prefix when it is called from pid 0 (terminal atm)
	// it uses interrupts
	// kprintf does neither of these,
    printf("Memory Information:\n");

    // Dump full memory map via kernel-side printing to the current stdout device
    dump_memory_map(stdout);
}