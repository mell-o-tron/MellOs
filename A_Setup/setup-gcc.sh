#!/usr/bin/env bash

# Get the distro name 
disto_release="/etc/os-release"
curr_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -f "$distro_release"]; then
    . "$distro_release"
else
    echo "Cannot get the distro name"
fi

if [[ "$ID" = "debian" || "$ID" = "ubuntu" || "$ID" = "linuxmint" || "$ID" = "pop" || "$ID" = "elementary" || "$ID" = "kali" ]]; then
    "$curr_dir/setup-gcc-debian.sh" 
elif [[ "$ID" = "arch" || "$ID" = "manjora" || "$ID" = "endeavouros" ]];then
    "$curr_dir/setup-gcc-debian.sh" 
else
    echo "Unsupported distro: $ID"
fi
