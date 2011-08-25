Why VirtualBox Only?
====================

I have learned from chats on IRC (#osdev on freenode) that there are two kinds of OS developer.  I happen to be a learner, the other type being a "serious" developer.  Anyway, I am writing this operating system as a learning experience for myself and others.  I am not trying to write the next Linux.  I am simply trying to learn some basics about what an operating system actually does.  In the process I am learning about all sorts of hardware, including the FDC, x86 chips, and the DMA controller.

floppyOS takes its name from a simple concept.  Currently, the only storage device that is at all supported by floppyOS is the Floppy Disk Controller.  The name also symbolises the true purpose of floppyOS.  Since no modern computer has an internal floppy drive, the name floppyOS signifies that the OS itself is not for use on computers.

What can it do?
===============

Very little.  The kernel itself contains a file ```kmain.c``` that contains the main routine for the OS.  This routine sets up some interrupts, programs the PIT, and then goes into an infinite loop.  This is a good setup for testing drivers and other subroutines because I simply need to add my test to the main function and I know that it will be run.

At the bottom of the ```kmain()``` function is a loop.  This loop reads a character from the keyboard, and outputs it to the screen.  If this loop is reached, you can type any ASCII letter or digit, or hit enter for a line feed.  In the future this functionality will be a polling system call and will allow multiple tasks to access the keyboard simultaniously.

The kernel does receive timer interrupts, and maintains a "system clock" of sorts.  This clock allows for delays and timeouts, but not much more.

Areas for development
---------------------

If you are looking for something that will suck all of your time away in an instant, I've got a solution for you!  There are several fields that need to be worked on:

* Additional hardware drivers for storage devices
* Filesystem support (maybe I will make a custom FS, who knows?)
* A better bootloader
* Multitasking or, rather, tasking at all would be nice!
* Software interrupts for user-level APIs

License
=======

FloppyOS is FREE SOFTWARE under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU General Public License</a>.
