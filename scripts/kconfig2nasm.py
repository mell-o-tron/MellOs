#!/usr/bin/env python3
"""
Convert auto.conf to NASM-compatible .inc file with CONFIG_ defines
"""
import sys
import re

def kconfig_to_nasm(input_file, output_file):
    """
    Reads auto.conf and writes a NASM .inc file with %define directives
    """
    with open(input_file, 'r') as f:
        lines = f.readlines()

    nasm_defines = []

    for line in lines:
        line = line.strip()

        # Skip comments and blank lines
        if not line or line.startswith('#'):
            continue

        # Parse CONFIG_FOO=value
        if line.startswith('CONFIG_'):
            match = re.match(r'CONFIG_([A-Za-z0-9_]+)=(.*)', line)
            if match:
                name, value = match.groups()

                # Handle different value types
                if value == 'y':
                    nasm_defines.append(f'%define CONFIG_{name} 1')
                elif value == 'm':
                    nasm_defines.append(f'%define CONFIG_{name} 2')
                elif value == 'n' or value == '':
                    # Skip unset values or define as 0
                    nasm_defines.append(f'%define CONFIG_{name} 0')
                else:
                    try:
                        if value.startswith('0x'):
                            nasm_defines.append(f'%define CONFIG_{name} {value}')
                        else:
                            int(value)
                            nasm_defines.append(f'%define CONFIG_{name} {value}')
                    except ValueError:
                        # String value - quote it
                        if value.startswith('"') and value.endswith('"'):
                            nasm_defines.append(f'%define CONFIG_{name} {value}')
                        else:
                            nasm_defines.append(f'%define CONFIG_{name} "{value}"')

    # Write NASM include file
    with open(output_file, 'w') as f:
        f.write('; Auto-generated NASM configuration from auto.conf\n')
        f.write('; DO NOT EDIT\n\n')
        for define in nasm_defines:
            f.write(define + '\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <auto.conf> <output.inc>")
        sys.exit(1)

    kconfig_to_nasm(sys.argv[1], sys.argv[2])