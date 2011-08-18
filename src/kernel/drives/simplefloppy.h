#include <kernel/kstdlib.h>
#include <kernel/drives/floppy.h>
#include <kernel/drives/dma.h>

osStatus floppy_setup ();
osStatus floppy_simple_read (unsigned char drive, unsigned char sector, unsigned char track, unsigned char head, unsigned char count, char * destination);
