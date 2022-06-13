# Phillip Kernel (PhillOS)
*A C++/C practice project* 
## Why PhillOS
This is a simple OS that is designed to be used as a learning tool, and actually once upon a time I wanted to build an OS for the fun (learning) of it. Eventually figured out how to do it, and then decided to make it.

Feel free to copy and paste this code into your own project.

## Some useful resources
Some of the resources I found most useful are [the OSDEV wiki](https://wiki.osdev.org/Main_Page), [osdever.net](http://www.osdever.net/tutorials/) (from which I basically stole the part regarding interrupts), this [keyboard scancode map](https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html), and of course the most fundamental of all resources: [RTFM!!!](https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-3a-3b-3c-and-3d-system-programming-guide.html)

## Future of PhillOS

From mello-tron's words:
 ( I'm doing this in my spare time, which basically means "during a small part of the summer and a week or so around Christmas". Feel free to do whatever you like with this, as long as you don't physically (nor psychologically) harm people or other animals I'm absolutely fine with it.
    Feel free to make pull requests and they'll be processed whenever possible.
 )

## Contributions
Special thanks to all of those who contributed. Your changelogs will be integrated in `contributions.md`

## Features
### Current
As simple as it can get:

- Simple bootloader, loads the kernel in memory and jumps to it
- Monolithic kernel
- Flat memory model (just the strictly necessary segmentation (data/code), no paging)
- Interrupt request handling, keyboard and timer support
- Linear dynamic memory allocation (with some tricks not to make it too inefficient)
- GDT (Different from the one in the Bootloader)


### Work in progress
- Dynamic, Modular Shell (will run executables when processes will be a thing)
- Floppy Disk Driver (does not work with USB floppy adapter)

### Future
- Process abstraction
- File System support, most likely USTAR (highly dependant on the floppy disk drivers)

### Bugs and Horrible Code
- The printy-screeney-bit is all broken and redundant, because it was made in a hurry for a very badly designed system. I updated most of the very badly designed system (it now is a mildly badly designed system), but not that, not yet. Next up, I guess.


### The idea of how to get this running
- Download the kernel and the bootloader
- Compile the kernel and the bootloader
- Copy the kernel and the bootloader to the floppy disk
- Boot the floppy disk

### Steps
- navigate to the root folder 
- run `sudo ./requirements.sh` (this installs all of the required build libraries and Visual Studio Code)
- run `sudo ./cross-compiler.sh` (make some tea/coffee/hot chocolate/etc)
- run `make` (make some more)
- every time you make some changes you can run `./commands.sh` to test the kernel
- run `make` some more to verify (this time it's fast)