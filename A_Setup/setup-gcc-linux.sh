#!/usr/bin/env bash

# Get the distro name 
distro_release="/etc/os-release"
curr_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
distros_setup_dir="$curr_dir/linux_distros"

if [ -f "$distro_release" ]; then
    . "$distro_release"
else
    echo "Cannot get the distro name"
    echo "File $distro_release not found."
    echo "Are you on linux?"
    echo "If you are having issues, file an issue on https://github.com/mell-o-tron/MellOs/issues"
    exit 1
fi

if [[ "$ID" = "debian" || "$ID" = "ubuntu" || "$ID" = "linuxmint" || "$ID" = "pop" || "$ID" = "elementary" || "$ID" = "kali" ]]; then
    cat "$distros_setup_dir/setup-gcc-debian.sh"
elif [[ "$ID" = "arch" || "$ID" = "manjaro" || "$ID" = "endeavouros" ]];then
    cat "$distros_setup_dir/setup-gcc-arch.sh" 
else
    echo "Unsupported distro: $ID"
    echo "Please file an issue on https://github.com/mell-o-tron/MellOs/issues"
    exit 1
fi
