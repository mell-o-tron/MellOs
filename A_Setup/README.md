## Table of Contents
- [Table of Contents](#table-of-contents)
- [Requirements](#requirements)
  - [Operating System:](#operating-system)
  - [Compiler:](#compiler)
  - [Linux Setup](#linux-setup)
  - [Easy setup:](#easy-setup)
- [Building](#building)
  - [CMake parameters](#cmake-parameters)
- [Running](#running)

----

## Requirements
### Operating System:
- [Linux](#linux-setup) (we have setup scripts for Debian and Arch based distros)
- Windows (easier with WSL)
- MacOS
- Other operating systems as long as they support all the tools you need

### Compiler:
You can use either a cross-compiling GCC (you need to build one) or Clang (easier).

### Linux Setup
You will need:
- CMake (to build)
- Clang or a cross-compiling GCC (to compile)
- Qemu (to run the operating system)
- NASM (assembler)
- mtools grub libisoburn (to generate the iso with the bootloader)

### Easy setup:
If you are using a Debian or Arch based distro, you can run `setup-linux.sh` in this folder to automatically install all dependencies and to compile a version of GCC that is able to compile the OS. Keep in mind that compiling GCC takes a long time, so either be prepared or use Clang for a faster setup.

**If you're using another distro, you may either use an emulator or make your own script (and make a pull request)**

---

## Building
Once you have all the necessary dependencies, go into the mellos directory, and run:

```sh
mkdir build
cd build
cmake <parameters> ..
cmake --build .
```

### CMake parameters
| Parameter            | Possible Values      | Description                                                                 |
| -------------------- | -------------------- | --------------------------------------------------------------------------- |
| `-DCMAKE_BUILD_TYPE` | `Debug`, `Release`   | Sets the build type to Debug for debugging or Release for optimized builds. |
| `-DUSE_CLANG`        | `ON`, `OFF`          | Enables or disables the use of Clang as the compiler.                       |
| `-DVGA`              | `TEXT`, `VESA`       | Sets the default video mode to either text mode or VESA graphics mode.      |
| `-DHRES`             | `<width>`            | Sets the horizontal resolution for VESA mode.                               |
| `-DVRES`             | `<height>`           | Sets the vertical resolution for VESA mode.                                 |
| `-DAUDIO_BACKEND`    | `NONE`, `PULSEAUDIO` | Sets the audio backend to use.                                              |
| `-DMACHINE`          | `PRESARIO`, `OTHER`  | Sets the target machine type.                                               |

Set this parameters when running the first `cmake` command. Example:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_CLANG=ON -DVGA=VESA -DHRES=800 -DVRES=600 -DMACHINE=OTHER ..
```
This will build MellOS in release mode, using Clang as the compiler, with VESA graphics mode at 800x600 resolution, targeting a generic machine.

---

## Running
After building, you can run MellOS using QEMU with the following command (from inside the `build` directory):
```sh
cmake --build . --target run
```