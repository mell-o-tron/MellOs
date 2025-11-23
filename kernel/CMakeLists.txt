enable_language(ASM_NASM)


set(KERNEL_C_SRCS stdio_devices.c ramfs.c pipe.c memory_mapper.c kernel_stdio.c
        kernel_graphics.c kernel.c file_descritor.c block_device_manager.c mount_manager.c file_string_formatting.c
        streams.c)
set(KERNEL_ASM_SRCS kernel_entry.asm IncBins.asm empty_end.asm)

add_library(kernel STATIC ${KERNEL_C_SRCS} ${KERNEL_ASM_SRCS})

string(APPEND CMAKE_ASM_NASM_FLAGS " -I${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(kernel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(kernel PRIVATE ${CMAKE_SOURCE_DIR}/global/include)
target_include_directories(kernel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal_includes)
target_link_libraries(kernel memory drivers data_structures shell_objs testing)
