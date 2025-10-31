# Usage:
#   collect_kconfig_cpu_flags(<out_var>)
#
# Example:
#   set(CPU_FLAGS "")
#   collect_kconfig_cpu_flags(CPU_FLAGS)
#   target_compile_options(kernel.elf PRIVATE ${CPU_FLAGS})
#
# Description:
#   Collects architecture- and feature-dependent compiler flags
#   into the given list variable (passed by name).
#   Adds only *positive* flags for features explicitly enabled in Kconfig.
function(collect_kconfig_cpu_flags out_var)
    if(NOT DEFINED CONFIG_ARCH_STRING)
        message(FATAL_ERROR "collect_kconfig_cpu_flags: CONFIG_ARCH_STRING undefined (did you include kconfig.cmake?)")
    endif()

    set(_march "${CONFIG_ARCH_STRING}")
    if(CONFIG_ARCH_STRING STREQUAL "qemu32")
        set(_march "i686")
    elseif(CONFIG_ARCH_STRING STREQUAL "generic32")
        set(_march "i686")
    elseif(CONFIG_ARCH_STRING STREQUAL "bochs")
        set(_march "i486")
    elseif(CONFIG_ARCH_STRING STREQUAL "pentium-mmx")
        set(_march "pentium-mmx")
    endif()

    set(_flags -m32 -march=${_march})

    if(CONFIG_CPU_FEAT_MMX)
        list(APPEND _flags -mmmx)
    else ()
        list(APPEND _flags -mno-mmx)
    endif()

    if(CONFIG_CPU_FEAT_SSE)
        list(APPEND _flags -msse)
    else ()
        list(APPEND _flags -mno-sse)
    endif()

    if(CONFIG_CPU_FEAT_SSE2)
        list(APPEND _flags -msse2)
    else ()
        list(APPEND _flags -mno-sse2)
    endif()

    if(CONFIG_CPU_FEAT_3DNOW AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
        list(APPEND _flags -m3dnow)
    else ()
        list(APPEND _flags -mno-3dnow)
    endif()

    if(DEFINED CONFIG_CPU_TUNE AND NOT CONFIG_CPU_TUNE STREQUAL "")
        list(APPEND _flags -mtune=${CONFIG_CPU_TUNE})
    endif()

    set(${out_var} ${${out_var}} ${_flags} PARENT_SCOPE)
endfunction()
