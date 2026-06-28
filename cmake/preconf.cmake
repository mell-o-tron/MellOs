# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 assembler-0.
# 
# This file is part of ZXFoundation (TM) and is ported to MellOS.
#  
# As part of MellOS, this file can be distributed and/or modified 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) 
# any later version.

string(TIMESTAMP CURRENT_YEAR "%Y")
string(TIMESTAMP CURRENT_MONTH "%m")

if(CURRENT_YEAR GREATER 2026)
    set(MellOs_Copyright_Date "2026-${CURRENT_YEAR}")
else()
    set(MellOs_Copyright_Date "2026")
endif()

set(MellOs_Release "${RELEASE_YY}${RELEASE_H}")

set(MellOs_Host_Build_Platform
    "${CMAKE_HOST_SYSTEM_NAME}@${CMAKE_HOST_SYSTEM_VERSION}::${CMAKE_HOST_SYSTEM_PROCESSOR}")

configure_file(
    ${CMAKE_SOURCE_DIR}/utils/preconf.h.in
    ${CMAKE_SOURCE_DIR}/utils/preconf.h
    @ONLY
)