cmake_minimum_required(VERSION 3.22)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS TRUE) # skip hosted link test
# set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS>
# <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Toolchain (can be overridden by -D on configure)
set(I386_PREFIX
        "/usr/local/i386elfgcc/bin"
        CACHE PATH "Path prefix where i386-elf tools are installed")
set(CMAKE_C_COMPILER
        "${I386_PREFIX}/i386-elf-gcc"
        CACHE FILEPATH "C compiler" FORCE)
# We let the gcc driver do the link but still pass linker script via link
# options
set(CMAKE_ASM_NASM_COMPILER
        "nasm"
        CACHE FILEPATH "NASM assembler")
set(OBJCOPY
        "${I386_PREFIX}/i386-elf-objcopy"
        CACHE FILEPATH "objcopy tool")
option(USE_CLANG "Use Clang as the compiler" OFF)

if(USE_CLANG)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_ASM_NASM_COMPILER "nasm")
    set(OBJCOPY "llvm-objcopy")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --target=i386-unknown-none-elf -march=i386")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --target=i386-unknown-none-elf -march=i386")
    set(CMAKE_LINKER "ld.lld" CACHE FILEPATH "Linker")
else()
    # Toolchain (can be overridden by -D on configure)
    set(I386_PREFIX "/usr/local/i386elfgcc/bin" CACHE PATH "Path prefix where i386-elf tools are installed")
    set(CMAKE_C_COMPILER    "${I386_PREFIX}/i386-elf-gcc" CACHE FILEPATH "C compiler" FORCE)
    # We let the gcc driver do the link but still pass linker script via link options
    set(CMAKE_ASM_NASM_COMPILER "nasm" CACHE FILEPATH "NASM assembler")
    set(OBJCOPY "${I386_PREFIX}/i386-elf-objcopy" CACHE FILEPATH "objcopy tool")
endif()

project(MellOs
 LANGUAGES C ASM_NASM
 HOMEPAGE_URL "https://github.com/mell-o-tron/MellOs"
)
enable_language(ASM_NASM)
set(MACHINE "OTHER" CACHE STRING "Machine target for the build. PRESARIO or OTHER")
set(VGA "VESA" CACHE STRING "Video mode macro VESA or TEXT")
set(HRES "600" CACHE STRING "Horizontal resolution")
set(VRES "400" CACHE STRING "Vertical resolution")
set(BPP  "32"   CACHE STRING "Bits per pixel")
set(WINDOW_DRAG, "NORMAL" STRING "NORMAL or BOX")
set(LDSCRIPT, "${CMAKE_SOURCE_DIR}/kernel/kernel.ld" CACHE STRING "Link script")
set(AUDIO_BACKEND "NONE" CACHE STRING "Audio backend to use: NONE or PULSEAUDIO")
set(ADDITIONAL_QEMU_FLAGS "" CACHE STRING "Additional flags to pass to QEMU when running")
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
# Helpful: export compile commands (a compile_commands.json will be in build
# dir)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(MellOs LANGUAGES C ASM_NASM)
# set(MACHINE "OTHER" CACHE STRING "Machine target for the build. PRESARIO or
# OTHER") set(TESTING "YES" CACHE STRING "Do you want to enable testing at
# runtime?") set(VGA "VESA" CACHE STRING "Video mode macro VESA or TEXT")
# set(HRES "600" CACHE STRING "Horizontal resolution") set(VRES "400" CACHE
# STRING "Vertical resolution") set(BPP  "32"   CACHE STRING "Bits per pixel")
# set(WINDOW_DRAG, "NORMAL" STRING "NORMAL or BOX")
set(LDSCRIPT,
        "${CMAKE_SOURCE_DIR}/kernel/kernel.ld"
        CACHE STRING "Link script")

set(ASM_SOURCES ${CMAKE_SOURCE_DIR}/cpu/gdt/gdt_loader.asm
        ${CMAKE_SOURCE_DIR}/processes/processes_asm.asm)

add_executable(kernel.elf ${C_SOURCES} ${ASM_SOURCES})

set(KCONFIG_ROOT ${CMAKE_SOURCE_DIR}/Kconfig)
set(KCONFIG_BUILD ${CMAKE_BINARY_DIR})
set(KCONFIG_DOTCONFIG ${KCONFIG_BUILD}/.config)
set(KCONFIG_HEADERDIR ${KCONFIG_BUILD}/include/generated)

set(KCONFIG_AUTOCONF ${KCONFIG_BUILD}/auto.conf)
set(KCONFIG_HEADER ${KCONFIG_HEADERDIR}/autoconf.h)
set(KCONFIG_NASM ${CMAKE_BINARY_DIR}/include/generated/kconfig.asm)
set(KCONFIG_CMAKE ${CMAKE_BINARY_DIR}/cmake/kconfig.cmake)

set(SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/scripts)
set(KCONFIG2CMAKE_EXE "${SCRIPTS_DIR}/kconfig2cmake.py")
set(OLDDEFCONFIG_EXE "${SCRIPTS_DIR}/kconfig-py/olddefconfig.py")
set(DEFCONFIG_EXE "${SCRIPTS_DIR}/kconfig-py/defconfig.py")
set(GENCONFIG_EXE "${SCRIPTS_DIR}/kconfig-py/genconfig.py")
set(KCONFIGLIB_EXE "${SCRIPTS_DIR}/kconfig-py/kconfiglib.py")
set(MENUCONFIG_EXE "${SCRIPTS_DIR}/kconfig-py/menuconfig.py")

include(${SCRIPTS_DIR}/helpers.cmake)

add_custom_target(
        menuconfig_gui
        COMMAND ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${KCONFIG_DOTCONFIG}
        ${MENUCONFIG_EXE} ${KCONFIG_ROOT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
        DEPENDS kconfig_all
        COMMENT "Launching Kconfig TUI (menuconfig)")

add_custom_command(
        OUTPUT ${KCONFIG_HEADER} ${KCONFIG_AUTOCONF}
        COMMAND
        ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${KCONFIG_DOTCONFIG} ${GENCONFIG_EXE}
        ${KCONFIG_ROOT} --header-path ${KCONFIG_HEADER} --config-out
        ${KCONFIG_BUILD}/auto.conf
        DEPENDS ${KCONFIG_ROOT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating ${KCONFIG_HEADER} from ${KCONFIG_DOTCONFIG}")

add_custom_target(kconfig_outputs DEPENDS ${KCONFIG_HEADER} ${KCONFIG_AUTOCONF})

add_custom_command(
        OUTPUT ${KCONFIG_CMAKE}
        COMMAND ${SCRIPTS_DIR}/kconfig2cmake.py ${KCONFIG_AUTOCONF} ${KCONFIG_CMAKE}
        DEPENDS ${KCONFIG_AUTOCONF} ${SCRIPTS_DIR}/kconfig2cmake.py
        COMMENT "Translating auto.conf to kconfig.cmake")

add_custom_command(
        OUTPUT ${KCONFIG_NASM}
        COMMAND ${SCRIPTS_DIR}/kconfig2nasm.py ${KCONFIG_AUTOCONF} ${KCONFIG_NASM}
        DEPENDS ${KCONFIG_AUTOCONF} ${SCRIPTS_DIR}/kconfig2nasm.py
        COMMENT "Translating auto.conf to kconfig.inc")

add_custom_target(kconfig_nasm DEPENDS kconfig_outputs ${KCONFIG_NASM})
add_custom_target(kconfig_cmake DEPENDS kconfig_outputs ${KCONFIG_CMAKE})

add_custom_target(kconfig_all DEPENDS kconfig_nasm kconfig_cmake)
add_dependencies(kernel.elf kconfig_all)

add_custom_target(
        menuconfig
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target menuconfig_gui
        --config $<CONFIG>
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target kconfig_all
        --config $<CONFIG>)

# Build flags similar to Makefile
set(COMMON_DEFS VGA_${VGA} HRES=${HRES} VRES=${VRES} BPP=${BPP} WINDOW_DRAG_${WINDOW_DRAG})
set(COMMON_DEFS_WITH_D)
foreach (def ${COMMON_DEFS})
    list(APPEND COMMON_DEFS_WITH_D "-D${def}")
endforeach ()
if (MACHINE STREQUAL "PRESARIO")
    list(APPEND COMMON_DEFS_WITH_D "-DDISABLE_SSE")
    set(DISABLE_SSE 1)
    set(TARGET_CPU "486")
else ()
    set(DISABLE_SSE 0)
    set(TARGET_CPU "qemu32")
endif()
#Add audio backend flags to qemu
if (AUDIO_BACKEND STREQUAL "PULSEAUDIO")
    list(APPEND ADDITIONAL_QEMU_FLAGS "-audiodev" "pa,id=snd0" "-machine" "pcspk-audiodev=snd0")
    list(APPEND COMMON_DEFS_WITH_D "-DAUDIO_ENABLED")
endif()
list(REMOVE_DUPLICATES COMMON_DEFS_WITH_D)

set_property(
        DIRECTORY
        APPEND
        PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${CMAKE_BINARY_DIR}/include/generated/autoconf.h")

set(CPU_FLAGS "")
execute_process(
        COMMAND ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${KCONFIG_DOTCONFIG}
        ${OLDDEFCONFIG_EXE} ${KCONFIG_ROOT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND_ERROR_IS_FATAL ANY)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_BINARY_DIR}/include/generated"
        COMMAND
        ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${KCONFIG_DOTCONFIG} ${GENCONFIG_EXE}
        ${KCONFIG_ROOT} --header-path ${KCONFIG_HEADER} --config-out
        ${KCONFIG_AUTOCONF}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND_ERROR_IS_FATAL ANY)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/cmake"
        COMMAND ${CMAKE_COMMAND} -E env KCONFIG_CONFIG=${KCONFIG_DOTCONFIG}
        ${KCONFIG2CMAKE_EXE} ${KCONFIG_AUTOCONF} ${KCONFIG_CMAKE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND_ERROR_IS_FATAL ANY)

execute_process(
        COMMAND ${SCRIPTS_DIR}/kconfig2nasm.py ${KCONFIG_AUTOCONF} ${KCONFIG_NASM}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND_ERROR_IS_FATAL ANY)

include(${KCONFIG_CMAKE})
include(${SCRIPTS_DIR}/kconfig_helper.cmake)

collect_kconfig_cpu_flags(CPU_FLAGS)
message(STATUS "cpu flags: ${CPU_FLAGS}")

# Map CONFIG_ARCH_STRING to QEMU -cpu value See scripts/kconfig_helper.cmake for
# how we map compiler -march; here we map for QEMU runtime Defaults to a broadly
# compatible CPU if unknown
if (NOT DEFINED CONFIG_ARCH_STRING)
    set(TARGET_CPU "qemu32")
else ()
    if (CONFIG_ARCH_STRING STREQUAL "i386")
        set(TARGET_CPU "386")
    elseif (CONFIG_ARCH_STRING STREQUAL "i486")
        set(TARGET_CPU "486")
    elseif (CONFIG_ARCH_STRING STREQUAL "i586" OR CONFIG_ARCH_STRING STREQUAL
            "pentium")
        set(TARGET_CPU "pentium")
    elseif (CONFIG_ARCH_STRING STREQUAL "i686")
        # Pick a safe i686-class CPU for QEMU; qemu32 is a generic 32-bit CPU
        set(TARGET_CPU "qemu32")
    elseif (CONFIG_ARCH_STRING STREQUAL "pentium-mmx")
        set(TARGET_CPU "pentium-mmx")
    elseif (CONFIG_ARCH_STRING STREQUAL "generic32" OR CONFIG_ARCH_STRING STREQUAL
            "qemu32")
        set(TARGET_CPU "qemu32")
    elseif (CONFIG_ARCH_STRING STREQUAL "bochs")
        # Bochs target is very old/strict; run QEMU in 486 mode
        set(TARGET_CPU "486")
    else ()
        # Fallback: try to use the arch string directly (works if it is a valid QEMU
        # CPU name)
        set(TARGET_CPU "${CONFIG_ARCH_STRING}")
    endif ()
endif ()
message(STATUS "QEMU target CPU: ${TARGET_CPU}")

include(${SCRIPTS_DIR}/helpers.cmake)

set(GENERATED_INC "${CMAKE_BINARY_DIR}/include/generated")
add_library(project_headers INTERFACE)
target_include_directories(project_headers INTERFACE "${GENERATED_INC}")
link_libraries(project_headers)

# Do not set global include directories to avoid cross-polluting libc includes
# (other than config)

add_subdirectory(vesa)
add_subdirectory(GUI)
add_subdirectory(shell)
add_subdirectory(text_editor)
add_subdirectory(kernel)
add_subdirectory(memory)
add_subdirectory(libc)
add_subdirectory(utils)
add_subdirectory(data_structures)
add_subdirectory(drivers)
add_subdirectory(cpu)
add_subdirectory(syscalls)
add_subdirectory(processes)
add_subdirectory(file_system)
add_subdirectory(disk_interface)
add_subdirectory(test)

set(NASM_INCS)
foreach (d ${INC_DIRS})
    list(APPEND NASM_INCS -I${d}/)
endforeach ()

list(APPEND NASM_INCS -I${CMAKE_BINARY_DIR}/include/generated)

set_target_properties(kernel.elf PROPERTIES OUTPUT_NAME kernel)

# target_include_directories(kernel.elf PRIVATE ${INC_DIRS}
# ${CMAKE_SOURCE_DIR}/kernel/include)
target_link_libraries(
        kernel.elf
        kernel
        drivers
        shell_objs
        gui_objs
        vesa_objects
        text_editor_objs
        memory
        cpu_api
        processes
        filesystem
        disk_interface
        utils
        syscalls
        mellos_libc
        data_structures
        testing)

# Add module object libraries
if (TARGET gui_objs)
    target_sources(kernel.elf PRIVATE $<TARGET_OBJECTS:mellos_libc>)
    # target_include_directories(gui_objs PRIVATE ${INC_DIRS}
    # ${CMAKE_SOURCE_DIR}/kernel/include)
endif ()

# if (TARGET shell_objs) target_sources(kernel.elf PRIVATE
# $<TARGET_OBJECTS:shell_objs>) target_include_directories(shell_objs PRIVATE
# ${INC_DIRS} ${CMAKE_SOURCE_DIR}/kernel/include) endif()

# if (TARGET text_editor_objs) target_sources(kernel.elf PRIVATE
# $<TARGET_OBJECTS:text_editor_objs>)
# target_include_directories(text_editor_objs PRIVATE ${INC_DIRS}
# ${CMAKE_SOURCE_DIR}/kernel/include) endif()

# Link libc for formatting functions used by non-kernel modules
# target_link_libraries(kernel.elf mellos_libc) Apply NASM flags correctly as a
# list (each arg stays its own token)
message(STATUS "nasm includes: ${NASM_INCS}")
foreach (asm ${ASM_SOURCES})
    set_source_files_properties(
            ${asm}
            PROPERTIES COMPILE_OPTIONS "${NASM_INCS};-w+all" # defs no longer needed,
            # settings in autoconf.asm
    )
endforeach ()

get_filename_component(LDSCRIPT "${CMAKE_SOURCE_DIR}/kernel/kernel.ld" ABSOLUTE)
# Linker script and options (similar to: ld -T kernel/kernel.ld ...) Use gcc
# driver to link with script and without standard libs
target_link_options(
        kernel.elf
        PRIVATE

if(USE_CLANG)
    target_link_options(kernel.elf PRIVATE
        -T${LDSCRIPT}
        -nostdlib
    )
else()
    target_link_options(kernel.elf PRIVATE
        -Wl,-T,${LDSCRIPT}
        -Wl,--start-group
        -no-pie
        -nostdlib
        -nodefaultlibs
        -nostartfiles
        -Wl,-m,elf_i386
        -Wl,--end-group)
endif()

# Freestanding kernel style build
# add '-DCMAKE_BUILD_TYPE=Debug' to your arguments for debug build
target_compile_options(kernel.elf PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-Og>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-g>
        )
# Freestanding kernel style build add '-DCMAKE_BUILD_TYPE=Debug' to your
# arguments for debug build
set(MELLOS_COMPILE_OPTIONS
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-O0>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-g3>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<CONFIG:Debug>>>:-O3>
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<BOOL:${USE_CLANG}>>>:-m32>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:${DISABLE_SSE}>>:-msse>
        $<$<COMPILE_LANGUAGE:C>:-nostdlib>
        $<$<COMPILE_LANGUAGE:C>:-nostdinc>
        $<$<COMPILE_LANGUAGE:C>:-fno-builtin>
        $<$<COMPILE_LANGUAGE:C>:-fstack-protector-strong>
        $<$<COMPILE_LANGUAGE:C>:-fno-pic>
        $<$<COMPILE_LANGUAGE:C>:-fno-strict-aliasing>
        $<$<COMPILE_LANGUAGE:C>:-fno-omit-frame-pointer>
        $<$<COMPILE_LANGUAGE:C>:-fno-optimize-sibling-calls>
        $<$<COMPILE_LANGUAGE:C>:-fno-delete-null-pointer-checks>
        $<$<COMPILE_LANGUAGE:C>:-Wall>
        $<$<COMPILE_LANGUAGE:C>:-Wno-parentheses>
        $<$<COMPILE_LANGUAGE:C>:-Wno-missing-braces>
        $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>
        $<$<COMPILE_LANGUAGE:C>:-Werror=return-type>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-DMELLOS_DEBUG>
        $<$<COMPILE_LANGUAGE:C>:${CPU_FLAGS}>)

target_compile_options(kernel.elf PRIVATE ${MELLOS_COMPILE_OPTIONS})

foreach (
        mod_t IN
        ITEMS gui_objs
        shell_objs
        text_editor
        drivers
        mellos_libc
        kernel
        vesa_objects
        memory
        data_structures
        utils
        syscalls
        processes
        testing)
    if (TARGET ${mod_t})
        target_compile_options(${mod_t} PRIVATE ${MELLOS_COMPILE_OPTIONS})
    endif ()
endforeach ()

# Create the ISO using grub-mkrescue
set(FD_NAME "mellos.fd")
set(ISO_NAME "mellos.iso")
set(ISO_PATH "${CMAKE_BINARY_DIR}/${ISO_NAME}")
set(FD_PATH "${CMAKE_BINARY_DIR}/${FD_NAME}")

set(ISO_ROOT "${CMAKE_BINARY_DIR}/iso_root")
set(KERNEL_DEST "${ISO_ROOT}/boot/kernel.elf")
set(GRUB_DIR "${ISO_ROOT}/boot/grub")


set(FD_ISO_ROOT "${CMAKE_BINARY_DIR}/floppy_iso_root")
set(FD_KERNEL_DEST "${FD_ISO_ROOT}/boot/kernel.elf")
set(FD_GRUB_DIR "${FD_ISO_ROOT}/boot/grub")

set(TEST_IMG_SOURCE "${CMAKE_SOURCE_DIR}/test_disk.img")
set(TEST_IMG_BINARY "${CMAKE_BINARY_DIR}/test_disk.img")

add_custom_target(
        prepare_grub_dir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GRUB_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/grub.cfg
        ${GRUB_DIR}/grub.cfg)

# Copy test_disk.img to build dir. Doing this to avoid situations where it is
# accidentally committed. This behaviour will change in the future
add_custom_command(
        OUTPUT ${TEST_IMG_BINARY}
        COMMAND ${CMAKE_COMMAND} -E copy ${TEST_IMG_SOURCE} ${TEST_IMG_BINARY}
        COMMENT "Copying test_disk.img to build directory")

set(GRUB_MODS "multiboot" "search" "search_fs_file" "biosdisk" "part_msdos")

set(GRUB_PLATFORM "i386-pc")
#add_grub_iso("mellos" "${GRUB_PLATFORM}" "/usr/lib/grub/${GRUB_PLATFORM}" "${KERNEL_DEST}" "${CMAKE_SOURCE_DIR}/grub.cfg")
add_custom_command(
        OUTPUT ${ISO_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN_DIR}
        COMMAND cp ${CMAKE_SOURCE_DIR}/cdpre.cfg /tmp/mellos-cdpre.cfg
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel.elf> ${KERNEL_DEST}
        #COMMAND grub-mkrescue -o ${ISO_PATH} --directory=/usr/lib/grub/i386-pc --install-modules=${GRUB_MODS} ${CMAKE_BINARY_DIR}/iso
        COMMAND grub-mkimage -O i386-pc
                -o /tmp/mellos-core-cd.img
                -p "(cd0)/boot/grub"
                -c /tmp/mellos-cdpre.cfg
                ${GRUB_MODS} "iso9660"


        COMMAND cat /usr/lib/grub/i386-pc/cdboot.img /tmp/mellos-core-cd.img > ${ISO_ROOT}/eltorito.img
        COMMAND ${CMAKE_COMMAND} -E chdir ${ISO_ROOT}
                xorriso -as mkisofs
                -o ${ISO_PATH}
                -b eltorito.img
                -no-emul-boot
                -boot-load-size 4
                -boot-info-table
                ${ISO_ROOT}/


        DEPENDS kernel.elf prepare_grub_dir ${CMAKE_SOURCE_DIR}/cdpre.cfg
        VERBATIM
        COMMENT "Creating bootable ISO with grub-mkimage and xorriso")

#todo: this is kinda jank and could be optimized
add_custom_command(
        OUTPUT ${FD_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN_DIR}
        COMMAND cp ${CMAKE_SOURCE_DIR}/fdpre.cfg /tmp/mellos-fdpre.cfg
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel.elf> ${FD_KERNEL_DEST}

        COMMAND grub-mkimage -O i386-pc
                -o /tmp/mellos-core-floppy.img
                -p "(fd0)/boot/grub"
                -c /tmp/mellos-fdpre.cfg
                ${GRUB_MODS} "fat"

        #COMMAND cat /usr/lib/grub/i386-pc/boot.img /tmp/mellos-core-floppy.img > ${FD_ISO_ROOT}/eltorito.img
        COMMAND dd if=/dev/zero of=mellos.fd bs=1024 count=1440

        COMMAND mformat -i ${FD_PATH} -f 1440 ::
        COMMAND mmd -i ${FD_PATH} ::/boot
        # COMMAND mmd -i ${FD_PATH} ::/boot/grub
        COMMAND mcopy -i ${FD_PATH} ${FD_ISO_ROOT}/boot/kernel.elf ::/boot/kernel.elf
        # todo: add build options for grub with normal mode/ stripped down grub
        # COMMAND mcopy -i ${FD_PATH} ${FD_ISO_ROOT}/boot/grub/grub.cfg ::/boot/grub/grub.cfg
        COMMAND dd if=/usr/lib/grub/i386-pc/boot.img of=mellos.fd conv=notrunc
        COMMAND dd if=/tmp/mellos-core-floppy.img of=mellos.fd bs=512 seek=1 conv=notrunc


        DEPENDS kernel.elf prepare_grub_dir ${CMAKE_SOURCE_DIR}/fdpre.cfg
        VERBATIM
        COMMENT "Creating bootable Floppy disk image with grub-mkimage and mutils")

add_custom_target(floppy ALL DEPENDS ${FD_PATH})
add_custom_target(iso ALL DEPENDS ${ISO_PATH})
add_custom_target(hda ALL DEPENDS ${TEST_IMG_BINARY})

# Debug symbol file equivalent to Makefile's kernel.sym
add_custom_target(
        debug_symbols
        COMMAND ${OBJCOPY} --only-keep-debug $<TARGET_FILE:kernel.elf>
        ${OUT_BIN_DIR}/kernel.sym
        DEPENDS kernel.elf
        COMMENT "Extracting debug symbols to kernel.sym")

# Run targets (mirroring Makefile)
add_custom_target(run
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -no-reboot -serial file:${CMAKE_BINARY_DIR}/serial.log -enable-kvm -cpu ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw ${ADDITIONAL_QEMU_FLAGS} -boot order=dc
add_custom_target(
        run
        COMMAND
        qemu-system-i386 -cdrom ${ISO_PATH} -serial
        file:${CMAKE_BINARY_DIR}/serial.log -monitor stdio -d int,cpu_reset -D
        ${CMAKE_BINARY_DIR}/qemu_debug.log -cpu ${TARGET_CPU}
        DEPENDS iso hda
        USES_TERMINAL
        COMMENT "Running MellOs in QEMU")

add_custom_target(
        debug
        COMMAND ${CMAKE_COMMAND} --build . --target debug_symbols
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -s -vga std -enable-kvm -cpu ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw ${ADDITIONAL_QEMU_FLAGS} -boot order=dc
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -gdb tcp::1234 -S -vga std -cpu
        ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw
        DEPENDS iso hda
        USES_TERMINAL
        COMMENT "Running QEMU with gdb stub (-s), std VGA and ${TARGET_CPU} cpu.")

add_custom_target(
        novga_telnet
        COMMAND ${CMAKE_COMMAND} --build . --target debug_symbols
        COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -s -enable-kvm -cpu ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw ${ADDITIONAL_QEMU_FLAGS} -boot order=dc -serial telnet:127.0.0.1:4444,server -nographic
        COMMAND
        qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -s -enable-kvm -cpu
        ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw -serial
        telnet:127.0.0.1:4444,server -nographic
        DEPENDS iso hda
        USES_TERMINAL
        COMMENT "Running QEMU with gdb stub (-s), no VGA, and bind UART I/O to telnet"
)
