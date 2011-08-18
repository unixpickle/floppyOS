#include <kernel/kpi.h>

typedef enum {
	FloppyCommandSpecify = 0x3,
	FloppyCommandWrite = 0x5,
	FloppyCommandRead = 0x6,
	FloppyCommandRecalibrate = 0x7,
	FloppyCommandSenseInterrupt = 0x8,
	FloppyCommandSeek = 0xf,
	FloppyCommandVersion = 0x10,
	FloppyCommandConfigure = 0x13,
	FloppyCommandUnlock = 0x14,
	FloppyCommandLock = 0x94
} FloppyCommand;

// floppy.s

/**
 * Returns the value of the 10th register in the CMOS.
 */
int floppy_read_CMOS ();

/**
 * Gives the number of drives for a CMOS floppy reading.
 */
int floppy_num_drives (int cmosReading);

/**
 * Detects if a reset is required.
 * @return 0 if no reset is required, 1 if reset is required.
 */
int floppy_needs_reset ();

/**
 * Reads and returns the value of the Digital Output Register
 */
int floppy_read_DOR ();

/**
 * Writes a new value to the Digital Output Register
 * @param newValue Only the first byte is written to the DOR
 */
void floppy_write_DOR (int newValue);

/**
 * Reads and returns the value of the FIFO port
 */
int floppy_read_FIFO ();

/**
 * Writes to the FIFO port.
 */
void floppy_write_FIFO (int fifoValue);

void floppy_write_DSR (int dsrVal);
void floppy_write_CCR (int dsrVal);

int floppy_read_MSR ();

// floppy.c

void floppy_sendbyte (unsigned char byte);
int floppy_wait_result ();

/**
 * Describes in a human readable format the type of drive that was
 * read from the CMOS.
 * @param cmosReading The value of register 0x10 from the CMOS
 * @param driveIndex The index of the drive (0 for master, 1 for slave)
 * @param destination A buffer at least 24 characters long.
 */
void floppy_describe (int cmosReading, int driveIndex, char * destination);

/**
 * Resets the floppy controller to its original state.
 * This should generally only need to be done once.
 */
void floppy_reset ();

/**
 * Selects a floppy drive using the DOR.
 */
void floppy_select (unsigned char disk, unsigned char specify);

/**
 * Spins up a floppy drive using the DOR.
 */
void floppy_spin_up (unsigned char disk);

void floppy_configure (unsigned char impliedSeek, unsigned char fifoDisable, unsigned char pollingMode, unsigned char threshold);

/**
 * Returns the result of a version command.  This should always be 0x90.
 */
int floppy_version ();

/**
 * Issues a sense interrupt.
 */
unsigned char floppy_sense_interrupt (unsigned char * currentCylinder);

/**
 * Recalibrate a drive (motor must be spinning).
 */
void floppy_recalibrate (unsigned char driveNumber);

/**
 * Specify the next drive that will be accessed.
 * @param SRT A.k.a the "Step Rate Time".  Safe value is 6ms.
 * @param HLT A.k.a the "Head Load Time".  safe value is 30ms.
 * @param HUT A.k.a the "Head Unload Time".  Safe value is 0.
 */
void floppy_specify (unsigned char SRT, unsigned char HLT, unsigned char HUT, unsigned char nDMA);

void floppy_seek (unsigned char head, unsigned char drive, unsigned char cylinder);

unsigned char floppy_read_write (FloppyCommand command, unsigned char head, unsigned char drive, unsigned char cylinder, unsigned char sector, unsigned char numSect, unsigned char gap);

void floppy_lock ();
void floppy_unlock ();
