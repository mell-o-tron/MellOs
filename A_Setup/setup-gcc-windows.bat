@echo off

REM Set environment variables for MinGW-w64
set MINGW_PREFIX=C:\mingw64
set PATH=%MINGW_PREFIX%\bin;%PATH%

REM Download sources
mkdir C:\tmp\src
cd C:\tmp\src
curl -o binutils-2.39.tar.gz http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
tar -xf binutils-2.39.tar.gz
mkdir binutils-build
cd binutils-build
..\binutils-2.39\configure --enable-interwork --enable-multilib --disable-nls --disable-werror 2>&1 | tee configure.log
make 2>&1 | tee make.log
make install 2>&1 | tee make_install.log
