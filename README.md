# Mellotron Kernel (MellOS)
*A minimal, collaboratively written kernel* 
## Why MellOS
A while back I was gifted a 1994 Compaq Presario, and I figured I could use it to learn how to write a simple operating system. I started writing some real mode programs, and eventually I moved on to writing a simple 32 bit kernel. 

I decided to share my results and progress for a couple of reasons, the main of which might be letting you have fun with this as much as I am, but also to share the ways I fixed my misconceptions, and the resources that helped me move on whenever I got stuck.

## Some useful resources
Some of the resources I found most useful are [the OSDEV wiki](https://wiki.osdev.org/Main_Page), [osdever.net](http://www.osdever.net/tutorials/) (from which I basically stole the part regarding interrupts), this [keyboard scancode map](https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html), and of course the most fundamental of all resources: [RTFM!!!](https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-3a-3b-3c-and-3d-system-programming-guide.html)

Another useful resource is the book "operating systems: principles and practice" by Thomas Anderson (no, not *that* Mr. Anderson) and Michael Dahlin. I used it to study for my OS class at university and I think it's a very good introduction to the subject.

## Future of MellOS
I'm doing this in my spare time, which basically means "during a small part of the summer and a week or so around Christmas". Feel free to do whatever you like with this, as long as you don't physically (nor psychologically) harm people or other animals I'm absolutely fine with it.

Feel free to make pull requests and they'll be processed whenever possible.

## Documentations
WIP, there's a markdown file in each directory, which will contain the docs. If you change or add something, try to integrate your change into the docs from now on. 

## Anything-here
If you make a useful change or addition, you are allowed to use the `anything_here` file to write whatever you feel like writing; rules are: 

- You may only add content, not delete
- Be polite
- Be creative

## Features
### Current

- Simple bootloader, loads the kernel in memory and jumps to it
- Monolithic kernel
- GDT (Different from the one in the Bootloader)
- Interrupt request handling, keyboard and timer support
- Paging
- Linear dynamic memory allocation (for the lower part of memory)
- Bitmap-based dynamic memory allocation (`kmalloc`/`krealloc`/`kfree`...)
- TTY-ish system to print to screen: allows to print to one of many canvases

### Work in progress
- Dynamic, Modular Shell (will run executables when processes will be a thing)
- File System
- Syscalls, flat binary loading

### Future
- Actual processes


