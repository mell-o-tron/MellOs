cmake_minimum_required(VERSION 3.22)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS TRUE)        # skip hosted link test
#set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Toolchain (can be overridden by -D on configure)
set(I386_PREFIX "/usr/local/i386elfgcc/bin" CACHE PATH "Path prefix where i386-elf tools are installed")
set(CMAKE_C_COMPILER    "${I386_PREFIX}/i386-elf-gcc" CACHE FILEPATH "C compiler" FORCE)
# We let the gcc driver do the link but still pass linker script via link options
set(CMAKE_ASM_NASM_COMPILER "nasm" CACHE FILEPATH "NASM assembler")
set(OBJCOPY "${I386_PREFIX}/i386-elf-objcopy" CACHE FILEPATH "objcopy tool")

project(MellOs C ASM_NASM)


# Options matching Makefile defaults
set(VGA "VESA" CACHE STRING "Video mode macro VESA or TEXT")
set(HRES "600" CACHE STRING "Horizontal resolution")
set(VRES "400" CACHE STRING "Vertical resolution")
set(BPP  "32"   CACHE STRING "Bits per pixel")
set(WINDOW_DRAG, "NORMAL" STRING "NORMAL or BOX")
set(LDSCRIPT, "${CMAKE_SOURCE_DIR}/kernel/kernel.ld" CACHE STRING "Link script")

#if (VGA STREQUAL "VESA")
#    add_compile_definitions(VGA_VESA)
#endif ()

#if (WINDOW_DRAG STREQUAL "BOX")
#    add_compile_definitions(BOX_WINDOW_DRAG)
#endif ()


# Output directory compatible with Makefile
set(OUT_BIN_DIR "${CMAKE_BINARY_DIR}/wee_bins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUT_BIN_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUT_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUT_BIN_DIR}")

# Build flags similar to Makefile
set(COMMON_DEFS VGA_${VGA} HRES=${HRES} VRES=${VRES} BPP=${BPP} WINDOW_DRAG_${WINDOW_DRAG})
set(COMMON_DEFS_WITH_D)
foreach (def ${COMMON_DEFS})
    list(APPEND COMMON_DEFS_WITH_D "-D${def}")
endforeach ()
list(REMOVE_DUPLICATES COMMON_DEFS_WITH_D)


#add_compile_definitions(${COMMON_DEFS})



# Keep includes behavior close to the original Makefile: include all dirs that have C files
file(GLOB_RECURSE KERNEL_C_SRCS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/*.c)
list(FILTER KERNEL_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

set(INC_DIRS)
foreach(src ${KERNEL_C_SRCS})
    get_filename_component(dir ${src} DIRECTORY)
    list(APPEND INC_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES INC_DIRS)
include_directories(${INC_DIRS})
# Source lists (replicate Makefile selection)
# C sources: all .c files

set(C_SOURCES ${KERNEL_C_SRCS})
# NASM setup (32-bit; pass flags as a LIST, not a concatenated string)


set(NASM_DEFS
        -D${VGA}
        -DHRES=${HRES}
        -DVRES=${VRES}
        -DBPP=${BPP}
)

set(NASM_INCS)
foreach(d ${INC_DIRS})
    list(APPEND NASM_INCS -i${d}/)
endforeach()

# Do NOT set CMAKE_ASM_NASM_FLAGS with a big string; it causes shell splitting
# set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} ${NASM_DEFS} ${NASM_INCS}")

# ASM sources
set(ASM_SOURCES
        ${CMAKE_SOURCE_DIR}/cpu/gdt/gdt_loader.asm
        ${CMAKE_SOURCE_DIR}/kernel/kernel_entry.asm
        ${CMAKE_SOURCE_DIR}/processes/processes_asm.asm
)

add_executable(kernel.elf ${C_SOURCES} ${ASM_SOURCES})
set_target_properties(kernel.elf PROPERTIES OUTPUT_NAME kernel)

# Apply NASM flags correctly as a list (each arg stays its own token)
foreach(asm ${ASM_SOURCES})
    set_source_files_properties(${asm} PROPERTIES
            COMPILE_OPTIONS "${NASM_DEFS};${NASM_INCS};-w+regsize"
    )
endforeach()

get_filename_component(LDSCRIPT "${CMAKE_SOURCE_DIR}/kernel/kernel.ld" ABSOLUTE)
# Linker script and options (similar to: ld -T kernel/kernel.ld ...)
# Use gcc driver to link with script and without standard libs
target_link_options(kernel.elf PRIVATE
        -Wl,-T,${LDSCRIPT}
        -nostdlib -nodefaultlibs -nostartfiles
        -Wl,-m,elf_i386
)
# Freestanding kernel style build
# add '-DCMAKE_BUILD_TYPE=Debug' to your arguments for debug build
target_compile_options(kernel.elf PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-Og>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-g>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<CONFIG:Debug>>>:-O3>
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C>:-m32>
        $<$<COMPILE_LANGUAGE:C>:-fno-builtin>
        $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
        $<$<COMPILE_LANGUAGE:C>:-fno-pic>
        $<$<COMPILE_LANGUAGE:C>:-Wall>
        $<$<COMPILE_LANGUAGE:C>:-Wno-parentheses>
        $<$<COMPILE_LANGUAGE:C>:-Wno-missing-braces>
        $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>
        $<$<COMPILE_LANGUAGE:C>:-Werror=return-type>
        $<$<COMPILE_LANGUAGE:C>:${COMMON_DEFS_WITH_D}>
)

# Create the ISO using grub-mkrescue
set(ISO_NAME "mellos.iso")
set(ISO_PATH "${CMAKE_BINARY_DIR}/${ISO_NAME}")
set(KERNEL_DEST "${CMAKE_BINARY_DIR}/iso/boot/kernel.elf")
set(GRUB_DIR "${CMAKE_BINARY_DIR}/iso/boot/grub")

add_custom_target(prepare_grub_dir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GRUB_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/grub.cfg ${GRUB_DIR}/grub.cfg
)

add_custom_command(
        OUTPUT ${ISO_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel.elf> ${KERNEL_DEST}
        COMMAND grub-mkrescue -o ${ISO_PATH} ${CMAKE_BINARY_DIR}/iso
        DEPENDS kernel.elf prepare_grub_dir
        VERBATIM
        COMMENT "Creating bootable ISO with grub-mkrescue"
)
add_custom_target(iso ALL DEPENDS ${ISO_PATH})

# Debug symbol file equivalent to Makefile's kernel.sym
add_custom_target(debug_symbols
        COMMAND ${OBJCOPY} --only-keep-debug $<TARGET_FILE:kernel.elf> ${OUT_BIN_DIR}/kernel.sym
        DEPENDS kernel.elf
        COMMENT "Extracting debug symbols to kernel.sym")

# Run targets (mirroring Makefile)
add_custom_target(run
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -no-reboot -serial file:${CMAKE_BINARY_DIR}/serial.log -monitor stdio -d int,cpu_reset -D ${CMAKE_BINARY_DIR}/qemu_debug.log -enable-kvm -cpu pentium2 -hda ${CMAKE_SOURCE_DIR}/test_disk.img
        DEPENDS iso
        USES_TERMINAL
        COMMENT "Running MellOs in QEMU")

add_custom_target(debug
        COMMAND ${CMAKE_COMMAND} --build . --target debug_symbols
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -s -vga std -enable-kvm -cpu pentium2 -hda ${CMAKE_SOURCE_DIR}/test_disk.img
        DEPENDS iso
        USES_TERMINAL
        COMMENT "Run QEMU with gdb stub (-s) and std VGA")

# Helpful: export compile commands (a compile_commands.json will be in build dir)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
