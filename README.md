Why VirtualBox Only?
====================

I have learned from chats on IRC (#osdev on freenode) that there are two kinds of OS developer.  I happen to be a learner, the other type being a "serious" developer.  Anyway, I am writing this operating system as a learning experience for myself and others.  I am not trying to write the next Linux.  I am simply trying to learn some basics about what an operating system actually does.  In the process I am learning about all sorts of hardware, including the FDC, x86 chips, and the DMA controller.

floppyOS takes its name from a simple concept.  Currently, the only storage device that is at all supported by floppyOS is the Floppy Disk Controller.  The name also symbolises the true purpose of floppyOS.  Since no modern computer has an internal floppy drive, the name floppyOS signifies that the OS itself is not for use on computers.

What can it do?
===============

Very little.  The kernel itself contains a file ```kmain.c``` that contains the main routine for the OS.  This routine sets up some interrupts, programs the PIT, and then starts a task using the current scheduling system.  This is a good setup for testing drivers and other subroutines.  All one needs to do to test their new driver is add some code to ```kmain()```.

Right now I am developing a small preemptive task scheduler.  This means that the kernel switches tasks using a timer interrupt, which is fired 100 times a second.  The current task system can successfully launch and maintain a task.  It probably can handle multiple tasks, but I haven't tested that yet.

Currently, the ```kmain()``` function launches machine code that it reads from disk.  This code contains an ```int 0x80``` call to print "Hello, task world!" to the console.

The kernel does receive timer interrupts, and maintains a "system clock" of sorts.  This clock allows for delays and timeouts, but not much more.

Areas for development
---------------------

If you are looking for something that will suck all of your time away in an instant, I've got a solution for you!  There are several fields that need to be worked on:

* Additional hardware drivers for storage devices
* Filesystem support
* A better bootloader
* Improved scheduling and lock system
* Software interrupts for user-level APIs

The Lock System
---------------

Any resources that may be shared between tasks need <i>locks</i>.  A lock allows for the system to reserve the use of a specific resource from all other processes.  For instance, the print system call locks a lock so that two processes cannot print at the same time.  If another process tries to print while one is already doing it, the second process will wait for the other one to unlock the lock.

The source file <tt>kernel/tasks/lock.h</tt> declares some kernel-level locking functions.  These functions include ```lock_vector``` and ```unlock_vector```.  Both of these methods take a number from 1 to 32, indicating which vector entry to lock.  Even though these methods may only be called by the kernel, they assume that a process triggered the kernel to lock the lock.  Normally these methods should only be called by ISRs or functions called only by ISRs.

You will also notice the ```lock_cpu()``` and ```unlock_cpu()``` functions.  These provide a simple recursive mechanism for locking and unlocking the current process.  After locking the CPU, a task switch will not take place until the CPU is unlocked.  This is handy if you want to receive hardware interrupts, but do not want task switches to occur.  Note that 256 ```lock_cpu()``` calls without matching unlock calls will cause the lock counter to wrap back to zero.

License
=======

FloppyOS is FREE SOFTWARE under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU General Public License</a>.
