#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include "dma.h"

/* Constants */
enum {
	FDC_SRA = 0x0,
	FDC_SRB = 0x1,
	FDC_DOR = 0x2,
	FDC_TDR = 0x3,
	FDC_MSR = 0x4, // read-only
	FDC_DSR = 0x4, // write-only
	FDC_FIFO = 0x5,
	FDC_DIR = 0x7, // read-only
	FDC_CCR = 0x7 // write-only
};

/* C structure definitions */
#include "floppytypes.h"

/* Assembly portion of the driver */
osStatus floppy_read_register (int registerNum, unsigned char * result);
osStatus floppy_write_register (int registerNum, int aByte);
osStatus floppy_read_cmos (unsigned char * destination);

/* C portion of the driver */
void handleFloppyCallback ();
osStatus floppy_wait_FIFO ();
osStatus floppy_read_FIFO (unsigned char * destination);
osStatus floppy_write_FIFO (unsigned char aByte);
osStatus floppy_get_list (FloppyDriveList * destination);
void floppy_describe (unsigned char cmosReading, FloppyDrive * aDrive);
osStatus floppy_get_string (unsigned char * destination, FloppyDrive * drive);

// floppy commands

osStatus floppy_version (unsigned char * version);
osStatus floppy_configure (bool impliedSeek, bool disableFifo, bool dpm, unsigned char threshold);
osStatus floppy_lock ();
osStatus floppy_unlock ();
osStatus floppy_reset ();
osStatus floppy_specify (unsigned char srt, unsigned char hlt, unsigned char hut, bool ndma, unsigned int datarate);
osStatus floppy_recalibrate (unsigned char drive);
osStatus floppy_sense_interrupt (unsigned char * st0, unsigned char * cylinder);
osStatus floppy_seek (unsigned char head, unsigned char drive, unsigned char cylinder);
osStatus floppy_read (unsigned char head, unsigned char drive, unsigned char cylinder, unsigned char sector, unsigned char count, char * destination);

// proceedures

osStatus floppy_drive_select (FloppyDrive * driveInfo, int index, bool needsSpecify);
osStatus floppy_drive_set_motor (int index, bool motOn);
osStatus floppy_drive_get_motor (int index, bool * destination);

