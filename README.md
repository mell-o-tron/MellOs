
# MellOS    [![Badge License]][License]

*The minimal, collaboratively written **Mellotron Kernel**.*

<br>

<div align = center>

---

[![Button Why]][Why]  
[![Button WIP]][WIP]  
[![Button Changes]][Changes]  
[![Button Bugs]][Bugs]

---

</div>

<br>

## Features

- ***Linear Dynamic Memory Allocation***

    *With some tricks not to make it too inefficient.*

- ***Simple Bootloader***
    
    *Loads the kernel in memory and jumps to it.*
    

- ***Flat Memory Model***
    
    *Just the strictly necessary segmentation* <br>
    *between data and code without paging.*


- ***Interrupt Request Handling***
    
    *With keyboard & timer support.*

- ***Monolithic kernel***

- ***GDT***
    
    *Separate from the Bootloaders one.*


<br>
<br>

## Resources

- **[Keyboard Scancodes]**

- **[OSDever.Net]**

    *I basically stole the part regarding interrupts*

- **[OSDEV Wiki]**

- **[RTFM]**

<br>
<br>

## Contributions

Special thanks to all of those who contributed.

Feel free to make pull requests, they <br> 
will be processed whenever possible.

<br>

### Spare Time

I'm doing this in my spare time, which basically <br>
means 'during a small part of the summer and <br>
a week or so around Christmas'.

<br>

<!-- Remove With Chosen License
Feel free to do whatever you like with this, as long <br>
as you don't physically (nor psychologically) harm <br>
people or other animals I'm absolutely fine with it. -->


<!----------------------------------------------------------------------------->

[Keyboard Scancodes]: https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
[OSDever.Net]: http://www.osdever.net/tutorials/
[OSDEV Wiki]: https://wiki.osdev.org/Main_Page
[RTFM]: https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-3a-3b-3c-and-3d-system-programming-guide.html

[Changes]: Documentation/Changelog.md
[License]: #
[Bugs]: Documentation/Bugs.md
[WIP]: Documentation/WorkInProgress.md
[Why]: Documentation/Why.md

[Badge License]: https://img.shields.io/badge/License-Unknown-808080.svg?style=for-the-badge


<!---------------------------------{ Buttons }--------------------------------->

[Button Changes]: https://img.shields.io/badge/Changelog-37bdb7?style=for-the-badge
[Button Bugs]: https://img.shields.io/badge/Bugs-bd4a37?style=for-the-badge
[Button WIP]: https://img.shields.io/badge/Work_In_Progress-71a4d7?style=for-the-badge
[Button Why]: https://img.shields.io/badge/Why-bda137?style=for-the-badge
