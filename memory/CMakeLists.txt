# Helpful: export compile commands (a compile_commands.json will be in build dir)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(memory STATIC
        paging/frame_allocator.c paging/paging.c paging/pat.c mem.c dynamic_mem.c process_memory.c
        stack_guard.c)
target_include_directories(memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(memory data_structures vesa_objects cpu_api kernel)
