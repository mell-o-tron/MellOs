export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

mkdir /tmp/src
cd /tmp/src
latest_binutils=$(curl http://ftp.gnu.org/gnu/binutils/ | tac | grep -o -m1 'binutils-[0-9]*\.[0-9]*\.*[0-9]*\.tar\.gz' | tail -1)
latest_binutils_directory=$(echo $latest_binutils | sed 's/\.tar\.gz//')
curl -O http://ftp.gnu.org/gnu/binutils/$latest_binutils
tar xf $latest_binutils
mkdir binutils-build
cd binutils-build
../$latest_binutils_directory/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
make all install 2>&1 | tee make.log


# cd /tmp/src
latest_gcc=$(curl http://ftp.gnu.org/gnu/gcc/ | tac | grep -o -m1 'gcc-[0-9]*\.[0-9]*\.*[0-9]*\.tar\.gz' | tail -1)
# curl -O https://ftp.gnu.org/gnu/gcc/gcc-12.1.0/gcc-12.1.0.tar.gz
latest_gcc_directory=$(echo $latest_gcc | sed 's/\.tar\.gz//')
curl -O http://ftp.gnu.org/gnu/gcc/$latest_gcc
# tar xf gcc-12.1.0.tar.gz
tar xf $latest_gcc
mkdir gcc-build
cd gcc-build
../$latest_gcc_directory/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c,c++ --without-headers
make all-gcc 
make all-target-libgcc 
make install-gcc 
make install-target-libgcc 