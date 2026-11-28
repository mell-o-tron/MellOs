# full kernel compilation

add_executable(kernel.elf ${KERNEL_C_SRCS} ${KERNEL_ASM_SRCS})
set_target_properties(kernel.elf PROPERTIES OUTPUT_NAME kernel)

set(LDSCRIPT "${CMAKE_SOURCE_DIR}/kernel/kernel.ld")

target_compile_options(kernel.elf PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -fno-builtin
        -fno-pic
        -Wall
        -Wno-parentheses
        -Wno-incompatible-pointer-types
        -Wno-missing-braces
        -ffreestanding
        -Werror=implicit-function-declaration
        -Werror=return-type
        -O${OPT_LEVEL}
        -g${DSYM_LEVEL}
        -m32
        -march=${MARCH_MODE}
        -mtune=${MARCH_MODE}
    >
    $<$<COMPILE_LANGUAGE:ASM_NASM>:
        -w+regsize
    >
)

target_link_options(kernel.elf PRIVATE
    -T ${LDSCRIPT}
    -nostdlib
    -static
    --no-dynamic-linker
    -ztext
    --no-pie
    -m${TARGET_EMULATION_MODE}
)

target_compile_definitions(kernel.elf PRIVATE
    ${CMMON_DEFS}
    $<$<CONFIG:Debug>:
        DMELLOS_DEBUG
    >
)

target_include_directories(kernel.elf PRIVATE SYSTEM
    ${INC_DIRS}
)

if (CONFIG_SSP)
    target_compile_options(kernel.elf PRIVATE
        $<$<COMPILE_LANGUAGE:C>:
            -fstack-protector-strong
        >
    )
endif()

if (NOT DISABLE_SSE)
    target_compile_options(kernel.elf PRIVATE
        $<$<COMPILE_LANGUAGE:C>:
            -msse
        >
    )
endif()

if (COMPILER_ID STREQUAL "clang")
    target_compile_options(kernel.elf PRIVATE
        $<$<COMPILE_LANGUAGE:C>:
            --target=${COMMON_TARGET_TRIPLE}
        >
    )
endif()

if (COMPILER_ID STREQUAL "gcc")
    target_link_options(kernel.elf PRIVATE
        --no-warn-rwx-segments
    )
endif()