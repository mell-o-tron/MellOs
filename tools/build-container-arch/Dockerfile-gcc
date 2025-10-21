# Dockerfile alex23087/mellos-build-arch:v0.1.1
FROM archlinux:base-devel-20251005.0.430597

# Most up-to-date setup script
ADD "https://raw.githubusercontent.com/mell-o-tron/MellOs/refs/heads/main/A_Setup/setup-gcc-arch.sh" "."
ENV MAKEFLAGS="-j$(nproc)"

# Make pacman non-interactive
RUN [ "sed", "-i", "s/pacman/pacman --noconfirm/g", "setup-gcc-arch.sh" ]

RUN [ "chmod", "+x", "setup-gcc-arch.sh" ]

# Update pacman repo cache before trying to install stuff
# These run commands are clustered together to cause less
# and smaller layers to be created.
RUN pacman -Sy && \
    ./setup-gcc-arch.sh && \
    pacman -Scc --noconfirm && \
    rm -f setup-gcc-arch.sh && \
    rm -rf /var/cache/pacman/pkg/* /tmp/* /var/tmp/*