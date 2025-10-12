FROM archlinux:base-devel-20251005.0.430597

# Most up-to-date setup script
ADD "https://raw.githubusercontent.com/mell-o-tron/MellOs/refs/heads/main/A_Setup/setup-gcc-arch.sh" "."
ENV MAKEFLAGS="-j$(nproc)"

# Make pacman non-interactive
RUN [ "sed", "-i", "s/pacman/pacman --noconfirm/g", "setup-gcc-arch.sh" ]

RUN [ "chmod", "+x", "setup-gcc-arch.sh" ]

# Update pacman repo cache before trying to install stuff
RUN [ "pacman", "-Sy" ]

RUN [ "./setup-gcc-arch.sh" ]