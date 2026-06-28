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

### Compiler (Toolchain setup):
You can use either a cross-compiling GCC (you need to build one) or Clang (easier).
You can pick one of the premade toolchains in `cmake/toolchains/`. We recommend [clang](../cmake/toolchains/mellos-clang.cmake)
Or you can make your own toolchain based on the premade ones

### Dependencies
- Clang/cross-GCC
- CMake
- GNU Make/Ninja
- GRUB with IA-32 support (optional)
- QEMU (optional)
- ccache (optional)

### GCC compilation scripts:
If you are using a Debian or Arch based distro, you can run `setup-linux.sh` in this folder to automatically install all dependencies and to compile a version of GCC that is able to compile the OS. Keep in mind that compiling GCC takes a long time, so either be prepared or use Clang for a faster setup.

**If you're using another distro, you may either use an emulator or make your own script (and make a pull request)**

---

## Building
Once you have all the necessary dependencies, go into the mellos directory, and run:

```sh
mkdir build
cd build
cmake -D<parameters> .. -DCMAKE_TOOLCHAIN_FILE=<your-toolchain>
cmake --build .
```

### CMake parameters
| Parameter          | Possible Values              | Description                                                                 |
|--------------------|------------------------------|-----------------------------------------------------------------------------|
| `CMAKE_BUILD_TYPE` | `Debug`, `Release`           | Sets the build type to Debug for debugging or Release for optimized builds. |
| `VGA`              | `TEXT`, `VESA`               | Sets the default video mode to either text mode or VESA graphics mode.      |
| `HRES`             | `<width>`                    | Sets the horizontal resolution for VESA mode.                               |
| `VRES`             | `<height>`                   | Sets the vertical resolution for VESA mode.                                 |
| `AUDIO_BACKEND`    | `NONE`, `PULSEAUDIO`         | Sets the audio backend to use.                                              |
| `MACHINE`          | `PRESARIO`, `OTHER`          | Sets the target machine type.                                               |
| `MARCH_MODE`       | `pentium` ...                | Parameter for compiler's `-march=` and `-mtune`. optional                   |
| `OPT_LEVEL`        | `0`, `1`, `2`, `3`, `s`, `z` | Optimization level (`-O`), optional                                         |
| `DSYM_LEVEL`       | `0`, `1`, `2`, `3`           | Debug symbol level (`-g`), optional                                         |

You can configure the project using the following example:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DVGA=VESA -DHRES=800 -DVRES=600 -DMACHINE=OTHER -DOPT_LEVEL=2 -DDSYM_LEVEL=0 -DMARCH_MODE=pentium ..
```

---

## Running
After building, you can run MellOS using QEMU with the following command (from inside the `build` directory):
```sh
cmake --build . --target run
```