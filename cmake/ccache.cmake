# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 assembler-0.
# 
# This file is part of ZXFoundation (TM) and is ported to MellOS.
#  
# As part of MellOS, this file can be distributed and/or modified 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) 
# any later version.

option(CCACHE_ENABLE "use ccache for faster compilation" ON)

if (CCACHE_ENABLE)
    if (CCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(CMAKE_ASM_COMPILER_LAUNCHER ${CCACHE_PROGRAM})

        set(ENV{CCACHE_MAXSIZE} "12G")

        message(STATUS "ccache enabled: ${CCACHE_PROGRAM}")

        execute_process(
            COMMAND ${CCACHE_PROGRAM} -s
            OUTPUT_VARIABLE CCACHE_STATS
            ERROR_QUIET
        )
        if (CCACHE_STATS)
            message(STATUS "ccache statistics:\n${CCACHE_STATS}")
        endif()
    else()
        message(WARNING "ccache requested but not found")
    endif()
endif()