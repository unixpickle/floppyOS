#include "floppy.h"

#define kFloppyHasIRQ6 (unsigned char *)0x505
#define kUsePIO false

void handleFloppyCallback () {
	unsigned char * irqBuf = kFloppyHasIRQ6;
	*irqBuf = 1;
}

osStatus floppy_wait_FIFO () {
	// give a 1 second timeout for reading the FIFO
	unsigned int kOvertime = kontime() + 1000;
	unsigned char msr = 0;
	while (floppy_read_register(FDC_MSR, &msr) == osOK) {
		if (kontime() > kOvertime) {
			return osTimeoutError;
		}
		// check RQM (set if it's OK to exchange bytes with the FIFO port).
		if ((msr & 0x80) == 0x80) {
			return osOK;
		}
	}
	return osGeneralError;
}

osStatus floppy_read_FIFO (unsigned char * destination) {
	if (floppy_wait_FIFO() != osOK) return osGeneralError;
	unsigned char msr = 0;
	if (floppy_read_register(FDC_MSR, &msr) != osOK) return osGeneralError;
	if ((msr & (0x80 | 0x40 | 0x10)) == (0x80 | 0x40 | 0x10)) {
		if (floppy_read_register(FDC_FIFO, destination) != osOK) {
			return osGeneralError;
		}
		return osOK;
	}
	return osGeneralError;
}

osStatus floppy_write_FIFO (unsigned char aByte) {
	if (floppy_wait_FIFO() != osOK) return osOK;
	unsigned char msr = 0;
	if (floppy_read_register(FDC_MSR, &msr) != osOK) return osGeneralError;
	if ((msr & (0x40 | 0x20 | 0x80)) == 0x80) {
		return floppy_write_register(FDC_FIFO, aByte);
	}
	return osGeneralError;
}

osStatus floppy_get_list (FloppyDriveList * destination) {
	// bzero so that no drives "exist".
	int i;
	for (i = 0; i < sizeof(FloppyDriveList); i++) {
		((char *)destination)[i] = 0;
	}
	unsigned char cmosReading = 0;
	if (floppy_read_cmos(&cmosReading) != osOK) return osGeneralError;
	// get first floppy descriptor
	unsigned char desc1 = (cmosReading >> 4) & 0xf;
	unsigned char desc2 = cmosReading & 0xf;
	if (desc1 != 0) {
		floppy_describe(desc1, &(destination->drives[0]));
	}
	if (desc2 != 0) {
		floppy_describe(desc2, &(destination->drives[1]));
	}
	return osOK;
}

void floppy_describe (unsigned char cmosReading, FloppyDrive * aDrive) {
	aDrive->cmosType = cmosReading;
	aDrive->supported = false;
	aDrive->exists = true;
	if (cmosReading == 1) {
		aDrive->datarate = 0;
		aDrive->motorDelay = 500;
	} else if (cmosReading == 2) {
		aDrive->datarate = 0;
		aDrive->motorDelay = 500;
	} else if (cmosReading == 3) {
		aDrive->datarate = 0;
	} else if (cmosReading == 4) {
		// currently the only supported floppy type
		aDrive->datarate = 0;
		aDrive->supported = true;
		aDrive->sectorsPerTrack = 18;
		aDrive->tracks = 80;
		aDrive->heads = 80;
		aDrive->motorDelay = 300;
	} else if (cmosReading == 5) {
		aDrive->datarate = 3;
	}
}

osStatus floppy_get_string (unsigned char * destination, FloppyDrive * drive) {
	const char * aDescription = "No drive";
	if (drive->exists == true) {
		switch (drive->cmosType) {
			case 1:
				aDescription = "360KB 3.25\" Drive";
				break;
			case 2:
				aDescription = "1.2MB 5.25\" Drive";
				break;
			case 3:
				aDescription = "720KB 3.5\" Drive";
				break;
			case 4:
				aDescription = "1.44MB 3.5\" Drive";
				break;
			case 5:
				aDescription = "2.88MB 3.5\" Drive";
				break;
		}
	}
	int len = strlen(aDescription), i;
	for (i = 0; i <= len; i++) {
		destination[i] = aDescription[i];
	}
}

osStatus floppy_version (unsigned char * version) {
	if (floppy_write_FIFO(FDC_CMD_VERSION) != osOK) return osGeneralError;
	return floppy_read_FIFO(version);
}

osStatus floppy_configure (bool impliedSeek, bool disableFifo, bool dpm, unsigned char threshold) {
	unsigned char cmdByte = (impliedSeek << 6) | (disableFifo << 5) | (dpm << 4) | (threshold - 1);
	if (floppy_write_FIFO(FDC_CMD_CONFIGURE) != osOK) return osGeneralError;
	if (floppy_write_FIFO(0) != osOK) return osGeneralError;
	if (floppy_write_FIFO(cmdByte) != osOK) return osGeneralError;
	if (floppy_write_FIFO(0) != osOK) return osGeneralError;
	return osOK;
}

osStatus floppy_lock () {
	if (floppy_write_FIFO(FDC_CMD_LOCK | 0x80) != osOK) return osGeneralError;
	unsigned char lockByte = 0;
	if (floppy_read_FIFO(&lockByte) != osOK) return osGeneralError;
	if ((lockByte & 0x10) == 0) return osGeneralError;
	return osOK;
}

osStatus floppy_unlock () {
	if (floppy_write_FIFO(FDC_CMD_LOCK) != osOK) return osGeneralError;
	unsigned char lockByte = 0;
	if (floppy_read_FIFO(&lockByte) != osOK) return osGeneralError;
	if ((lockByte & 0x10) != 0) return osGeneralError;
	return osOK;
}

osStatus floppy_reset () {
	unsigned char * hasIRQ = kFloppyHasIRQ6;

	*hasIRQ = 0;
	// TODO: insert good datarate here.
	floppy_write_register(FDC_DSR, 0x80);
	
	unsigned int waitUntil = kontime() + 20, i;
	while (!*hasIRQ) {
		if (kontime() > waitUntil) return osTimeoutError;
	}
	for (i = 0; i < 4; i++) {
		unsigned char st0;
		unsigned char cyl;
		if (floppy_sense_interrupt(&st0, &cyl) != osOK) {
			return osGeneralError;
		}
	}
	return osOK;
}

osStatus floppy_specify (unsigned char srt, unsigned char hlt, unsigned char hut, bool ndma, unsigned int datarate) {
	// calculate the SRT, HLT, and HUT values
	unsigned int srt_val = (16 - (srt * datarate / 500000));
	unsigned int hlt_val = hlt * datarate / 1000000;
	unsigned int hut_val = hut * datarate / 8000000;
	unsigned char param1 = ((srt_val << 4) | hut_val);
	unsigned char param2 = ((hlt_val << 1) | ndma);
	if (floppy_write_FIFO(FDC_CMD_SPECIFY) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param1) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param2) != osOK) return osGeneralError;
	return osOK;
}

osStatus floppy_recalibrate (unsigned char drive) {
	unsigned char * hasIRQ = kFloppyHasIRQ6;
	*hasIRQ = 0;

	if (floppy_write_FIFO(FDC_CMD_RECALIBRATE) != osOK) return osGeneralError;
	if (floppy_write_FIFO(drive) != osOK) return osGeneralError;
	int waitUntil = kontime() + 3010;
	while (!(*hasIRQ)) {
		if (kontime() > waitUntil) return osTimeoutError;
	}
	return osOK;
}

osStatus floppy_sense_interrupt (unsigned char * st0, unsigned char * cylinder) {
	if (floppy_write_FIFO(FDC_CMD_SENSE_INTERRUPT) != osOK) {
		return osGeneralError;
	}
	if (floppy_read_FIFO(st0) != osOK) return osGeneralError;
	if (floppy_read_FIFO(cylinder) != osOK) return osGeneralError;
	return osOK;
}

osStatus floppy_seek (unsigned char head, unsigned char drive, unsigned char cylinder) {
	unsigned char * hasIRQ = kFloppyHasIRQ6;
	*hasIRQ = 0;

	unsigned char param1 = (head << 2) | drive;
	unsigned char param2 = cylinder;
	if (floppy_write_FIFO(FDC_CMD_SEEK) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param1) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param2) != osOK) return osGeneralError;
	unsigned int waitUntil = kontime() + 3000;
	while (!(*hasIRQ)) {
		if (kontime() > waitUntil) {
			return osTimeoutError;
		}
	}
	return osOK;
}

osStatus floppy_read (unsigned char head, unsigned char drive, unsigned char cylinder, unsigned char sector, unsigned char count, char * destination) {
	unsigned char msr = 0;
	floppy_read_register(FDC_MSR, &msr);
	unsigned char * hasIRQ = kFloppyHasIRQ6;
	*hasIRQ = 0;
	if ((msr & 0xc0) != 0x80) return osGeneralError;
	unsigned char param1 = (head << 2) | drive;
	unsigned char param2 = cylinder;
	unsigned char param3 = head;
	unsigned char param4 = sector;
	unsigned char param5 = 2;
	unsigned char param6 = count;
	unsigned char param7 = 0x1b; // gap1
	unsigned char param8 = 0xff;
	if (floppy_write_FIFO(0x80 | 0x40 | FDC_CMD_READ_DATA) != osOK) {
		return osGeneralError;
	}
	if (floppy_write_FIFO(param1) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param2) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param3) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param4) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param5) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param6) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param7) != osOK) return osGeneralError;
	if (floppy_write_FIFO(param8) != osOK) return osGeneralError;
	while (!(*hasIRQ)) {
	}
	unsigned char st0;
	unsigned char st1;
	unsigned char st2;
	unsigned char bogus;
	if (floppy_read_FIFO(&st0) != osOK) return osGeneralError;
	if (floppy_read_FIFO(&st1) != osOK) return osGeneralError;
	if (floppy_read_FIFO(&st2) != osOK) return osGeneralError;
	if (floppy_read_FIFO(&bogus) != osOK) return osGeneralError;
	if (floppy_read_FIFO(&bogus) != osOK) return osGeneralError;
	if (floppy_read_FIFO(&bogus) != osOK) return osGeneralError;
	if (floppy_read_FIFO(&bogus) != osOK) return osGeneralError;
	return osOK;
}

// BEGIN: proceedures

osStatus floppy_drive_select (FloppyDrive * driveInfo, int index, bool needsSpecify) {
	int datarate = 0;
	if (driveInfo->datarate == 0) datarate = 500000;
	if (driveInfo->datarate == 3) datarate = 1000000;
	if (floppy_write_register(FDC_CCR, driveInfo->datarate) != osOK) {
		return osGeneralError;
	}
	if (needsSpecify == true) {
		if (floppy_specify(8, 30, 240, kUsePIO, datarate) != osOK) {
			return osGeneralError; // general error
		}
	}
	unsigned char dorValue = 0;
	if (floppy_read_register(FDC_DOR, &dorValue) != osOK) return osGeneralError;
	dorValue &= 0xfc;
	dorValue |= index;
	return osOK;
}

osStatus floppy_drive_set_motor (int index, bool motOn) {
	unsigned char dorValue = 0;
	unsigned char motValue = (0x10 << index);
	if (floppy_read_register(FDC_DOR, &dorValue) != osOK) return osGeneralError;
	if (motOn) {
		dorValue |= motValue;
	} else {
		dorValue &= (0xff ^ motValue);
	}
	if (floppy_write_register(FDC_DOR, dorValue) != osOK) {
		return osGeneralError;
	}

	// wait 500ms for the drive to spin up/down
	int waitUntil = kontime() + 500;
	while (kontime() < waitUntil) { }

	return osOK;
}

osStatus floppy_drive_get_motor (int index, bool * destination) {
	unsigned char dorValue = 0;
	unsigned char motValue = (0x10 << index);
	if (floppy_read_register(FDC_DOR, &dorValue) != osOK) return osGeneralError;
	if ((dorValue & motValue) != 0) {
		*destination = false;
	} else {
		*destination = true;
	}
	return osOK;
}

