
# MellOS    [![Badge License]][License]

*The minimal, collaboratively written **Mellotron Kernel**.*

<br>

## Why

A while back I was gifted a `1994 Compaq Presario` <br>
and figured I could use it to learn how to write a <br>
simple operating system.

I started writing some real mode programs and <br>
eventually I moved on to a simple 32-bit kernel. 

I decided to share my results and progress for a <br>
couple of reasons, the main of which might be <br>
letting you have fun with this as much as I am.

I also wanted to share the ways by which I fixed <br>
my misconceptions, as well as the resources that <br>
helped me move on whenever I got stuck.

<br>

## Resources

- **[Keyboard Scancodes]**

- **[OSDever.Net]**

    *I basically stole the part regarding interrupts*

- **[OSDEV Wiki]**

- **[RTFM]**

<br>

## Future of MellOS
I'm doing this in my spare time, which basically means "during a small part of the summer and a week or so around Christmas". Feel free to do whatever you like with this, as long as you don't physically (nor psychologically) harm people or other animals I'm absolutely fine with it.

Feel free to make pull requests and they'll be processed whenever possible.

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
- Floppy Disk Driver (does not work with USB floppy adapter, so I can only work on it when I'm at home, which is quite rare considering I live a fairly long way from my uni)

### Future
- Process abstraction
- File System support, most likely USTAR (highly dependant on the floppy disk drivers)

### Bugs and Horrible Code
- The printy-screeney-bit is all broken and redundant, because it was made in a hurry for a very badly designed system. I updated most of the very badly designed system (it now is a mildly badly designed system), but not that, not yet. Next up, I guess.



<!----------------------------------------------------------------------------->

[Keyboard Scancodes]: https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
[OSDever.Net]: http://www.osdever.net/tutorials/
[OSDEV Wiki]: https://wiki.osdev.org/Main_Page
[RTFM]: https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-3a-3b-3c-and-3d-system-programming-guide.html

[License]: #

[Badge License]: https://img.shields.io/badge/License-Unknown-808080.svg?style=for-the-badge