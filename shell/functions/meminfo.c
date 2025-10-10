//
// Created by matias on 9/25/25.
//

#include "stdio.h"
#include "mellos/kernel/memory_mapper.h"

#define FDEF(name) void name(const char* s)

FDEF(meminfo){
    printf("Memory Information:\n");

    const MemoryArea memory_area = get_largest_free_block();
    printf("selected: %016llx..%016llx\n", (uint64_t)memory_area.start, (uint64_t)memory_area.start + (uint64_t)memory_area.length);
}