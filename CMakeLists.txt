cmake_minimum_required(VERSION 3.16)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)   # CMake will pass exactly "-f elf32"

project(MellOs C ASM_NASM)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS TRUE)        # skip hosted link test
#set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Options matching Makefile defaults
set(VGA "VGA_VESA" CACHE STRING "Video mode macro (e.g., VGA_VESA)")
set(HRES "1920" CACHE STRING "Horizontal resolution")
set(VRES "1080" CACHE STRING "Vertical resolution")
set(BPP  "32"   CACHE STRING "Bits per pixel")
set(LDSCRIPT, "${CMAKE_SOURCE_DIR}/kernel/kernel.ld" CACHE STRING "Link script")


# Toolchain (can be overridden by -D on configure)
set(I386_PREFIX "/usr/local/i386elfgcc/bin" CACHE PATH "Path prefix where i386-elf tools are installed")
set(CMAKE_C_COMPILER    "${I386_PREFIX}/i386-elf-gcc" CACHE FILEPATH "C compiler" FORCE)
# We let the gcc driver do the link but still pass linker script via link options
set(CMAKE_ASM_NASM_COMPILER "nasm" CACHE FILEPATH "NASM assembler")
set(OBJCOPY "${I386_PREFIX}/i386-elf-objcopy" CACHE FILEPATH "objcopy tool")

# Output directory compatible with Makefile
set(OUT_BIN_DIR "${CMAKE_SOURCE_DIR}/wee_bins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUT_BIN_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUT_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUT_BIN_DIR}")

# Build flags similar to Makefile
set(COMMON_DEFS ${VGA} HRES=${HRES} VRES=${VRES} BPP=${BPP})
add_compile_definitions(${COMMON_DEFS})



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
target_compile_options(kernel.elf PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-g>
        $<$<COMPILE_LANGUAGE:C>:-O3>
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C>:-m32>
        $<$<COMPILE_LANGUAGE:C>:-fno-builtin>
        $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
        $<$<COMPILE_LANGUAGE:C>:-fno-pic>
)

# Create the ISO using grub-mkrescue
set(ISO_NAME "mellos.iso")
set(ISO_PATH "${CMAKE_SOURCE_DIR}/${ISO_NAME}")
set(KERNEL_DEST "${CMAKE_SOURCE_DIR}/iso/boot/kernel.elf")

add_custom_command(
        OUTPUT ${ISO_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel.elf> ${KERNEL_DEST}
        COMMAND grub-mkrescue -o ${ISO_PATH} ${CMAKE_SOURCE_DIR}/iso
        DEPENDS kernel.elf ${CMAKE_SOURCE_DIR}/iso/boot/grub/grub.cfg
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
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -no-reboot -serial file:${CMAKE_SOURCE_DIR}/serial.log -monitor stdio -d int,cpu_reset -D ${CMAKE_SOURCE_DIR}/qemu_debug.log -hda ${CMAKE_SOURCE_DIR}/test_disk.img
        DEPENDS iso
        USES_TERMINAL
        COMMENT "Running MellOs in QEMU")

add_custom_target(debug
        COMMAND ${CMAKE_COMMAND} --build . --target debug_symbols
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -hda ${CMAKE_SOURCE_DIR}/test_disk.img -m 128M -s -vga std
        DEPENDS iso
        USES_TERMINAL
        COMMENT "Run QEMU with gdb stub (-s) and std VGA")

# Convenience clean for wee_bins artifacts
add_custom_target(distclean
        COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_BINARY_DIR}/cmake_install.cmake
        ${CMAKE_BINARY_DIR}/serial.log
        ${CMAKE_BINARY_DIR}/CMakeCache.txt
        ${CMAKE_SOURCE_DIR}/wee_bins
        ${CMAKE_SOURCE_DIR}/iso/boot/kernel.elf
        ${CMAKE_SOURCE_DIR}/mellos.iso
        COMMENT "Remove generated binaries and ISO"
)

# Helpful: export compile commands (a compile_commands.json will be in build dir)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
