
# MellOS    [![Badge License]][License]

*The minimal, collaboratively written **Mellotron Kernel**.*

<br>



<br>

## Resources

- **[Keyboard Scancodes]**

- **[OSDever.Net]**

    *I basically stole the part regarding interrupts*

- **[OSDEV Wiki]**

- **[RTFM]**

<br>



## Contributions

Special thanks to all of those who contributed. <br>
Your changelogs will be integrated in `contributions.md`

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


<!----------------------------------------------------------------------------->

[Keyboard Scancodes]: https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
[OSDever.Net]: http://www.osdever.net/tutorials/
[OSDEV Wiki]: https://wiki.osdev.org/Main_Page
[RTFM]: https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-3a-3b-3c-and-3d-system-programming-guide.html

[License]: #

[Badge License]: https://img.shields.io/badge/License-Unknown-808080.svg?style=for-the-badge