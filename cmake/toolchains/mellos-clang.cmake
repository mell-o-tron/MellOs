# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 assembler-0.
# 
# This file is part of ZXFoundation (TM) and is ported to MellOS.
#  
# As part of MellOS, this file can be distributed and/or modified 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) 
# any later version.


if (NOT DEFINED ENV{CLANG_VERSION})
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_ASM_COMPILER clang)
else()
    set(CMAKE_ENV_CLANG_VERSION "$ENV{CLANG_VERSION}")
    set(CMAKE_C_COMPILER clang-${CMAKE_ENV_CLANG_VERSION})
    set(CMAKE_CXX_COMPILER clang++-${CMAKE_ENV_CLANG_VERSION})
    set(CMAKE_ASM_COMPILER clang-${CMAKE_ENV_CLANG_VERSION})
endif()

set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_LINKER ld.lld)
set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_OBJCOPY llvm-objcopy)
set(MOS_NM llvm-nm)
set(MOS_CXXFILT llvm-cxxfilt)
set(MOS_HOST_CC clang)

set(COMPILER_ID "clang")
set(MARCH_MODE "pentium")

set(OPT_LEVEL "2" CACHE STRING "Optimization level (0, 1, 2, 3, s, z)")
set(DSYM_LEVEL "0" CACHE STRING "Debug symbol level (0, 1, 2, 3)")