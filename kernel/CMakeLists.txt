enable_language(ASM_NASM)


set(KERNEL_C_SRCS stdio_devices.c filesystem/ramfs.c pipe.c memory_mapper.c kernel_stdio.c
        kernel_graphics.c kernel.c filesystem/file_descritor.c filesystem/block_device_manager.c filesystem/mount_manager.c filesystem/file_string_formatting.c
        streams.c
        filesystem/dentry.c
        algorithms/djb2.c
        boot_parsing.c)
set(KERNEL_ASM_SRCS kernel_entry.asm IncBins.asm empty_end.asm
        include/mellos/hash.h)

add_library(kernel STATIC ${KERNEL_C_SRCS} ${KERNEL_ASM_SRCS})

string(APPEND CMAKE_ASM_NASM_FLAGS " -I${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(kernel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(kernel PRIVATE ${CMAKE_SOURCE_DIR}/global/include)
target_include_directories(kernel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal_includes)
target_link_libraries(kernel memory drivers data_structures shell_objs testing)
