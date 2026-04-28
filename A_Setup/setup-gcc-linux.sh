#!/usr/bin/env bash

# --- Setup Paths ---
# Using $(dirname ...) is the best way to make the script work 
# no matter where you call it from.
BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DISTRO_SCRIPTS="$BASE_DIR/linux_distros"
OS_INFO="/etc/os-release"

# --- Style Definitions ---
BOLD='\033[1m'
BLUE='\033[34m'
RED='\033[31m'
NC='\033[0m' # No Color

echo -e "${BLUE}${BOLD}==> Initializing Environment Setup...${NC}"

# 1. Check if we are actually on Linux
if [ ! -f "$OS_INFO" ]; then
    echo -e "${RED}[!] Error: /etc/os-release not found. Is this a Linux system?${NC}"
    exit 1
fi

# Load the OS info
. "$OS_INFO"

# 2. Distro Detection & Script Routing
case "$ID" in
    debian|ubuntu|linuxmint|pop|elementary|kali)
        TARGET_SCRIPT="$DISTRO_SCRIPTS/setup-gcc-debian.sh"
        ;;
    arch|manjaro|endeavouros)
        TARGET_SCRIPT="$DISTRO_SCRIPTS/setup-gcc-arch.sh"
        ;;
    *)
        echo -e "${RED}[!] Unsupported Distro: $ID${NC}"
        echo "Please report issues at: https://github.com/mell-o-tron/MellOs/issues"
        exit 1
        ;;
esac

# 3. Execution Phase
if [ -f "$TARGET_SCRIPT" ]; then
    echo -e "${BLUE}==> Detected Distro: ${BOLD}$ID${NC}"
    echo -e "${BLUE}==> Launching: $(basename "$TARGET_SCRIPT")${NC}"
    
    # Source the script (runs in current shell context)
    . "$TARGET_SCRIPT"
else
    echo -e "${RED}[!] Error: Configuration script missing at $TARGET_SCRIPT${NC}"
    exit 1
fi
