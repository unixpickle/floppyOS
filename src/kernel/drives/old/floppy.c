#include "floppy.h"

extern void kprint (const char * str);

#define kIRQ6FlagAddress 0x505

void floppy_sendbyte (unsigned char byte) {
	while ((floppy_read_MSR() & 0x80) == 0) {
	}
	floppy_write_FIFO((int)byte);
}

int floppy_wait_result () {
	int waittill = kontime() + 3000;
	while (1) {
		if ((floppy_read_MSR() & (0x10 | 0x80)) == (0x10 | 0x80)) {
			return 0;
		}
		if (kontime() > waittill) break;
	}
	return -1;
}

/**
 * writes the description of a floppy drive to a character buffer.
 * @param cmosReading The numeric reading from CMOS register 0x10
 * @param driveIndex The index of the drive to describe.  This can
 * be either 0 or 1.
 * @param destination The description destination.  Must be at least
 * 24 bytes long.
 */
void floppy_describe (int cmosReading, int driveIndex, char * destination) {
	int i;
	if (driveIndex < 0 || driveIndex > 1) return;
	unsigned char myReading = cmosReading & 0xf;
	const char * descriptionc = "No drive";
	if (driveIndex == 0) {
		myReading = (cmosReading >> 4) & 0xf;
	}
	switch (myReading) {
		case 1:
			descriptionc = "360 KB 5.25 Drive";
			break;
		case 2:
			descriptionc = "1.2 MB 5.25 Drive";
			break;
		case 3:
			descriptionc = "720 KB 3.5 Drive";
			break;
		case 4:
			descriptionc = "1.44 MB 3.5 Drive";
			break;
		case 5:
			descriptionc = "2.88 MB 3.5 Drive";
			break;
		default:
			break;
	}
	for (i = 0; i < 24; i++) {
		destination[i] = descriptionc[i];
		if (descriptionc[i] == 0) {
			break;
		}
	}
	return;
}

int floppy_num_drives (int cmosValue) {
	int count = 0;
	if ((cmosValue & 0xf) != 0) count += 1;
	if (((cmosValue >> 4) & 0xf) != 0) count += 1;
	return count;
}

void floppy_reset () {
	unsigned char * irq6Boolean = (unsigned char *)kIRQ6FlagAddress;
	*irq6Boolean = 0;
	int oldDor = floppy_read_DOR();
	if (oldDor == 12) {
		kprint("olddir: 12  ");
	} else if (oldDor == 28) {
		kprint("olddir: 28  ");
	} else {
		kprint("no oldDOR  ");
	}
	floppy_write_DOR(0);
	int waitTime = kontime() + 20;
	while (kontime() < waitTime) {
	}
	floppy_write_DOR(oldDor);
	while (*irq6Boolean == 0) {
	}
	floppy_sendbyte(FloppyCommandSpecify);
	floppy_sendbyte(0xdf);
	floppy_sendbyte(0x02);
}

void floppy_select (unsigned char drive, unsigned char specify) {
	floppy_write_CCR(0);
	if (specify != 0) {
		floppy_specify(8, 15, 0, 0);
	}
	// select that sucker.
	floppy_write_DOR((floppy_read_DOR() | drive));
}

void floppy_spin_up (unsigned char drive) {
	if (drive == 0) {
		floppy_write_DOR((floppy_read_DOR() | 0x10));
	} else if (drive == 1) {
		floppy_write_DOR((floppy_read_DOR() | 0x20));
	}
	int waittil = kontime() + 500;
	while (kontime() < waittil) {
	}
}

void floppy_configure (unsigned char impliedSeek, unsigned char fifoDisable, unsigned char pollingMode, unsigned char threshold) {
	unsigned char cmdByte = 0;
	cmdByte = (impliedSeek << 6) | (fifoDisable << 5) | (pollingMode << 4) | (threshold - 1);
	floppy_sendbyte(FloppyCommandConfigure);
	floppy_sendbyte(0);
	floppy_sendbyte(cmdByte);
	floppy_sendbyte(0);
}

int floppy_version () {
	floppy_sendbyte(FloppyCommandVersion);
	// wait for 2 ms before reading
	return floppy_read_FIFO();
}

unsigned char floppy_sense_interrupt (unsigned char * currentCylinder) {
	floppy_sendbyte(FloppyCommandSenseInterrupt);
	while (1) {
		if ((floppy_read_MSR() & 0x80) != 0) {
			break;
		}
	}
	floppy_wait_result();
	unsigned char resultByte = (unsigned char)floppy_read_FIFO();
	if (currentCylinder) *currentCylinder = (unsigned char)floppy_read_FIFO();
	return resultByte;
}

void floppy_recalibrate (unsigned char driveNumber) {
	unsigned char * irq6Boolean = (unsigned char *)kIRQ6FlagAddress;
	*irq6Boolean = 0;
	floppy_sendbyte(FloppyCommandRecalibrate);
	floppy_sendbyte(driveNumber);
	while (!*irq6Boolean) {
	}
}

void floppy_specify (unsigned char SRT, unsigned char HLT, unsigned char HUT, unsigned char nDMA) {
	floppy_write_FIFO(FloppyCommandSpecify);
	floppy_write_FIFO((SRT << 4) | HUT);
	floppy_write_FIFO((HLT << 1) | nDMA);
}

void floppy_seek (unsigned char head, unsigned char drive, unsigned char cylinder) {
	unsigned char * irq6Boolean = (unsigned char *)kIRQ6FlagAddress;
	*irq6Boolean = 0;
	floppy_write_FIFO(FloppyCommandSeek);
	floppy_write_FIFO((head << 2) | drive);
	floppy_write_FIFO(cylinder);
	while (!*irq6Boolean) {
	}
}

unsigned char floppy_read_write (FloppyCommand command, unsigned char head, unsigned char drive, unsigned char cylinder, unsigned char sector, unsigned char numSect, unsigned char gap) {
	unsigned char * irq6Boolean = (unsigned char *)kIRQ6FlagAddress;
	*irq6Boolean = 0;
	floppy_write_FIFO((0x80 | 0x40 | command));
	floppy_write_FIFO((head << 2) | drive);
	floppy_write_FIFO(cylinder);
	floppy_write_FIFO(head);
	floppy_write_FIFO(sector);
	floppy_write_FIFO(2); // all floppy drives use 512 bytes/sector
	floppy_write_FIFO(numSect);
	floppy_write_FIFO(gap); // 0x1b by default
	floppy_write_FIFO(0xff); // x+2 * 2 = 512
	while (1) {
		if (*irq6Boolean != 0 && (floppy_read_MSR() & 0x80) != 0) {
			break;
		}
	}
	floppy_read_FIFO();
	floppy_read_FIFO();
	floppy_read_FIFO();
	floppy_read_FIFO();
	floppy_read_FIFO();
	floppy_read_FIFO();
	return floppy_read_FIFO();
}

void floppy_lock () {
	floppy_write_FIFO(FloppyCommandLock);
	floppy_read_FIFO();
}

void floppy_unlock () {
	floppy_write_FIFO(FloppyCommandUnlock);
	floppy_read_FIFO();
}

/**
 * IRQ6 handler function
 */
void handleFloppyCallback () {
	kprint("Floppy callback\n");
	unsigned char * irq6Boolean = (unsigned char *)kIRQ6FlagAddress;
	*irq6Boolean = 1;
}

