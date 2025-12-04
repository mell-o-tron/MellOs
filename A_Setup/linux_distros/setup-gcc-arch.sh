#!/usr/bin/env bash
sudo pacman -S nasm qemu cmake dnsmasq vde2 bridge-utils openbsd-netcat base-devel bison flex gmp libmpc mpfr texinfo mtools grub libisoburn
export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

# Change directory with error checking: warn the user and abort if cd fails
cd_warn_abort() {
  local dir="$1"
  if ! cd "$dir"; then
    echo "Error: failed to change directory to '$dir'. Aborting." >&2
    exit 1
  fi
}

mkdir /tmp/src
cd_warn_abort /tmp/src
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
tar xf binutils-2.39.tar.gz
mkdir binutils-build
cd_warn_abort binutils-build
../binutils-2.39/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
sudo make all install 2>&1 | tee make.log

cd_warn_abort /tmp/src
curl -O https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xf gcc-12.2.0.tar.gz
mkdir gcc-build
cd_warn_abort gcc-build
echo Configure: . . . . . . .
../gcc-12.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-language=c,c++ --without-headers
echo MAKE ALL-GCC:
sudo make all-gcc
echo MAKE ALL-TARGET-LIBGCC:
sudo make all-target-libgcc
echo MAKE INSTALL-GCC:
sudo make install-gcc
echo MAKE INSTALL-TARGET-LIBGCC:
sudo make install-target-libgcc
echo HERE U GO MAYBE:
ls /usr/local/i386elfgcc/bin
export PATH="$PATH:/usr/local/i386elfgcc/bin"
