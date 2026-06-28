# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 assembler-0.
# 
# This file is part of ZXFoundation (TM) and is ported to MellOS.
#  
# As part of MellOS, this file can be distributed and/or modified 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) 
# any later version.

if (NOT DEFINED ENV{GCC_PREFIX})
    set(CMAKE_C_COMPILER i386-elf-gcc)
    set(CMAKE_CXX_COMPILER i386-elf-g++)
    set(CMAKE_ASM_COMPILER i386-elf-gcc)
    set(CMAKE_LINKER i386-elf-ld)
    set(CMAKE_AR i386-elf-ar)
    set(CMAKE_RANLIB i386-elf-ranlib)
    set(CMAKE_OBJCOPY i386-elf-objcopy)
    set(MOS_NM i386-elf-nm)
    set(MOS_CXXFILT i386-elf-c++filt)
else()
    set(CMAKE_ENV_GCC_PREFIX "$ENV{GCC_PREFIX}")
    set(CMAKE_C_COMPILER ${CMAKE_ENV_GCC_PREFIX}-gcc)
    set(CMAKE_CXX_COMPILER ${CMAKE_ENV_GCC_PREFIX}-g++)
    set(CMAKE_ASM_COMPILER ${CMAKE_ENV_GCC_PREFIX}-gcc)
    set(CMAKE_LINKER ${CMAKE_ENV_GCC_PREFIX}-ld)
    set(CMAKE_AR ${CMAKE_ENV_GCC_PREFIX}-ar)
    set(CMAKE_RANLIB ${CMAKE_ENV_GCC_PREFIX}-ranlib)
    set(CMAKE_OBJCOPY ${CMAKE_ENV_GCC_PREFIX}-objcopy)
    set(MOS_NM ${CMAKE_ENV_GCC_PREFIX}-nm)
    set(MOS_CXXFILT ${CMAKE_ENV_GCC_PREFIX}-c++filt)
endif()

set(MOS_HOST_CC gcc)
set(COMPILER_ID "gcc")
set(MARCH_MODE "pentium")

set(OPT_LEVEL "2" CACHE STRING "Optimization level (0, 1, 2, 3, s, z)")
set(DSYM_LEVEL "0" CACHE STRING "Debug symbol level (0, 1, 2, 3)")