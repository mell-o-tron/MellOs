#!/bin/bash
# add_autoconf_include.sh

# Default values
DRY_RUN=false
MODIFIED_COUNT=0
SKIPPED_COUNT=0

# Help function
show_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Add #include "autoconf.h" to files that reference CONFIG_* macros but don't have the include.

OPTIONS:
    -h, --help          Show this help message and exit
    --dry-run           Show what would be modified without making changes

EXAMPLES:
    $0                  Add includes to all matching files
    $0 --dry-run        Preview changes without modifying files
    $0 -h               Show this help message

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

if [ "$DRY_RUN" = true ]; then
    echo "=== DRY RUN MODE - No files will be modified ==="
    echo ""
fi

find . -type f \( -name "*.c" -o -name "*.h" \) \
    ! -path "./build/*" \
    ! -path "./.git/*" | while read file; do

    if grep -q "CONFIG_[A-Z_]*" "$file"; then
        if ! grep -q '#include.*"autoconf.h"' "$file" && ! grep -q '#include.*<autoconf.h>' "$file"; then
            echo "Adding #include \"autoconf.h\" to $file"

            if grep -q "^#pragma once" "$file"; then
                # after #pragma once
                sed -i '/^#pragma once/a #include "autoconf.h"' "$file"
            elif grep -q "^#ifndef" "$file"; then
                # after #ifndef
                sed -i '/^#ifndef/a #include "autoconf.h"' "$file"
            else
                # at the beginning
                sed -i '1i #include "autoconf.h"' "$file"
            fi
        else
            SKIPPED_COUNT=$((SKIPPED_COUNT + 1))
        fi
    fi
done

echo ""
echo "=== Summary ==="
if [ "$DRY_RUN" = true ]; then
    echo "Files that would be modified: $MODIFIED_COUNT"
else
    echo "Files modified: $MODIFIED_COUNT"
fi
echo "Files skipped (already have include): $SKIPPED_COUNT"
echo ""

if [ "$DRY_RUN" = true ]; then
    echo "To apply changes, run: $0"
else
    echo "Done!"
fi