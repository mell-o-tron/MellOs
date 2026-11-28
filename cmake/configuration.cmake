# configurations

if (NOT DEFINED OPT_LEVEL)
    set(OPT_LEVEL "2" CACHE STRING "Optimization level (0, 1, 2, 3, s, z)")
endif()

if (NOT DEFINED DSYM_LEVEL)
    set(DSYM_LEVEL "0" CACHE STRING "Debug symbol level (0, 1, 2, 3)")
endif()

if (NOT DEFINED MARCH_MODE)
    set(MARCH_MODE "pentium" CACHE STRING "Argument for -march= and -mtune=")
endif()

option(CONFIG_SSP "Enable ssp for kernel" ON)

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

# Build flags similar to Makefile
set(COMMON_DEFS ${VGA} VGA_${VGA} HRES=${HRES} VRES=${VRES} BPP=${BPP} WINDOW_DRAG_${WINDOW_DRAG})
if (MACHINE STREQUAL "PRESARIO")
    set(TARGET_CPU "486")
    add_compile_definitions(DISABLE_SSE)
else ()
    set(TARGET_CPU "386") # we defaults to i386 toolchain
endif()

# Add audio backend flags to qemu
if (AUDIO_BACKEND STREQUAL "PULSEAUDIO")
    list(APPEND ADDITIONAL_QEMU_FLAGS "-audiodev" "pa,id=snd0" "-machine" "pcspk-audiodev=snd0")
    add_compile_definitions(AUDIO_ENABLED)
endif()

