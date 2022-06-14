#!/bin/bash
export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

# mkdir /tmp/src
# cd /tmp/src
# latest_binutils=$(curl https://ftp.gnu.org/gnu/binutils/ | tac | grep -o -m1 'binutils-[0-9]*\.[0-9]*\.*[0-9]*\.tar\.gz' | tail -1)
# latest_binutils_directory=$(echo $latest_binutils | sed 's/\.tar\.gz//')
# curl -O http://ftp.gnu.org/gnu/binutils/$latest_binutils
# tar xf $latest_binutils
# mkdir $latest_binutils_directory
# cd $latest_binutils_directory
# ../$latest_binutils_directory/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
# make all install 2>&1 | tee make.log

mkdir /tmp/src
cd /tmp/src
latest_gcc_version=$(curl http://ftp.gnu.org/gnu/gcc/ | tac | grep -o -m1 'gcc-[0-9]*\.[0-9]*\.*[0-9]*' | tail -1)
latest_gcc_file="$latest_gcc_version.tar.gz"
latest_gcc_version_exists=$(curl -s -o /dev/null -w "%{http_code}" http://ftp.gnu.org/gnu/gcc/$latest_gcc_file)
if [ $latest_gcc_version_exists == "404" ]; then
    latest_gcc_uri="$latest_gcc_version/$latest_gcc_file"
elif [ $latest_gcc_version_exists == "200" ]; then
    latest_gcc_uri=$latest_gcc_file
else
    echo "Error: Could not determine latest gcc version"
    exit 1
fi
latest_gcc_directory=$latest_gcc_version
curl -O http://ftp.gnu.org/gnu/gcc/$latest_gcc_uri
tar xf $latest_gcc_file
mkdir $latest_gcc_directory
cd $latest_gcc_directory
../$latest_gcc_directory/contrib/download_prerequisites
../$latest_gcc_directory/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c,c++ --without-headers
make all-gcc 
make all-target-libgcc 
make install-gcc 
make install-target-libgcc 