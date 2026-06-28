# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 assembler-0.
# 
# This file is part of ZXFoundation (TM) and is ported to MellOS.
#  
# As part of MellOS, this file can be distributed and/or modified 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) 
# any later version.

cmake_minimum_required(VERSION 3.0...3.10)
include(cmake/meta.cmake)
project(MellOs
	LANGUAGES C ASM_NASM
	HOMEPAGE_URL "https://github.com/mell-o-tron/MellOs"
)
enable_language(ASM_NASM)

if (NOT CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "build: no toolchain file specified")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/toolchain")

include(preconf)              # Pre-configuration
include(dependencies) 		  # Host dependencies
include(ccache)				  # CCache configuration
include(configuration) 		  # Build/kernel options
include(standard)   	      # C Standards
include(sources)        	  # Core kernel sources
include(mellos-compile)       # Full kernel compilation
include(run)       		      # Run targets (mostly post-build targets)
