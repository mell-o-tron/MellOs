#!/bin/bash

export PREFIX="/usr/local/i386elfgcc"
export PREFIX_GRUB="/usr/local/grub-2_14"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"


if [[ $1 == '-h' || $1 == '--help' ]]; then
    echo ''
    echo '  setup-gcc-suse.sh'
    echo ''
    echo '      --clean'
    echo '  remove all the directories and binaries created'
    echo '  and the sources downloaded'
    echo ''
    echo '      --help -h'
    echo '  prints this'
    echo ''
    exit 0
fi;

if [[ $(id -u) -ne 0 ]]; then
    echo 'This script needs to be run as root.'
    exit 1
fi;

if [[ $1 == '--clean' ]]; then
    sudo rm -rf $PREFIX $PREFIX_GRUB /tmp/src
    exit 0
fi;

run_noperm() {
    if [[ $SUDO_USER -ne "root" ]]; then
        exec "$1"
        return 0
    fi;
    sudo -n -u $SUDO_USER -- bash -lc "$*"
}



zypper --non-interactive update

zypper --non-interactive install gcc gcc-c++ make cmake qemu-x86 python315 bison flex curl mpc-devel mpfr-devel gmp-devel mtools xorriso nasm grub2-i386-pc

run_noperm "echo '=========================================='
          echo '   COMPILING AND INSTALLING BINUTILS'
          echo '=========================================='"

run_noperm mkdir /tmp/src
cd /tmp/src
run_noperm "curl -O 'http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz'
          tar xf binutils-2.39.tar.gz 2>&1 > binutils-tar.log
          cd binutils-2.39
          echo '=========================================='
          echo ''
          ./configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 > binutils-configure.log"
run_noperm "echo ''"
cd binutils-2.39
make all install 2>&1 | tee binutils-make.log

run_noperm "echo '=========================================='
          echo '     COMPILING AND INSTALLING GCC'
          echo '=========================================='"

cd /tmp/src
run_noperm "curl -O 'https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz'
          tar xf gcc-12.2.0.tar.gz 2>&1 | tee gcc-tar.log
          cd gcc-12.2.0
          echo '=========================================='
          echo ''
          ./configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-language=c,c++ --without-headers 2>&1 > gcc-configure.log
          echo ''"
cd gcc-12.2.0
make all-gcc install-gcc 2>&1 | tee gcc-make.log

run_noperm "echo '=========================================='
          echo ''
          echo '        HERE U GO MAYBE:'
          ls $PREFIX/bin"
export PATH="$PATH:$PREFIX/bin"
run_noperm "echo '=========================================='"
