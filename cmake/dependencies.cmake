# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 assembler-0.
# 
# This file is part of ZXFoundation (TM) and is ported to MellOS.
#  
# As part of MellOS, this file can be distributed and/or modified 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) 
# any later version.

find_program(CCACHE_PROGRAM ccache)
find_program(QEMU_SYS qemu-system-i386)

# Grub comes in different names across distros, for example:
# Fedora is grub2-*
# Brew is i686-elf-grub-* (legacy BIOS)
find_program(GRUB_MKRESCUE
    NAMES
        grub-mkrescue
        grub2-mkrescue
        i686-elf-grub-mkrescue
    DOC "Path to the grub-mkrescue executable"
)