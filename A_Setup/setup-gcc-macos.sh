#!/bin/bash

#Requirements: Brew and default zsh user

# Exit immediately if a command exits with a non-zero status
set -e

echo "📦 Installing Homebrew packages..."
brew install gcc make bison flex gmp libmpc mpfr texinfo wget curl nasm qemu

# Ensure Homebrew's Bison is used
echo 'export PATH="/usr/local/opt/bison/bin:$PATH"' >> ~/.zshrc

# Set environment variables
export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

# Make these exports permanent in zsh
echo 'export PREFIX="/usr/local/i386elfgcc"' >> ~/.zshrc
echo 'export TARGET=i386-elf' >> ~/.zshrc
echo 'export PATH="$PREFIX/bin:$PATH"' >> ~/.zshrc

# Create source directory
mkdir -p /tmp/src
cd /tmp/src

echo "🛠️ Downloading and building Binutils..."
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
tar xf binutils-2.39.tar.gz
mkdir binutils-build
cd binutils-build
../binutils-2.39/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX
make -j$(sysctl -n hw.ncpu)
sudo make install

cd /tmp/src

echo "🛠️ Downloading and building GCC..."
curl -O https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xf gcc-12.2.0.tar.gz
mkdir gcc-build
cd gcc-build
../gcc-12.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c,c++ --without-headers
make all-gcc -j$(sysctl -n hw.ncpu)
make all-target-libgcc -j$(sysctl -n hw.ncpu)
sudo make install-gcc
sudo make install-target-libgcc

echo "✅ Installation complete! Verifying..."
ls /usr/local/i386elfgcc/bin
echo "🔍 Checking GCC version..."
i386-elf-gcc --version

source ~/.zshrc
