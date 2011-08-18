#include "simplefloppy.h"

static osStatus floppy_recal (FloppyDrive aDrive, int index) {
	int i;
	// select drive
	if (floppy_drive_select(&aDrive, index, true) != osOK) {
		return osGeneralError;
	}
	// detect the motor
	bool motor = false;
	if (floppy_drive_get_motor(index, &motor) != osOK) return osGeneralError;
	// make sure it's on
	if (!motor) {
		if (floppy_drive_set_motor(index, true) != osOK) return osGeneralError;
	}
	// recalibrate it (attempt three times)
	unsigned char st0;
	unsigned char cylinder;
	for (i = 0; i < 3; i++) {
		if (floppy_recalibrate(index) != osOK) {
			return osGeneralError;
		}
		if (floppy_sense_interrupt(&st0, &cylinder) != osOK) {
			return osGeneralError;
		}
		if ((st0 & 0x20) != 0) break;
		if (i == 2) return osGeneralError; // tried three times, all failed.
	}
	return osOK;
}

osStatus floppy_setup () {
	int i;
	// detect the version of the subsystem
	// unsigned char version = 0;
	// if (floppy_version(&version) != osOK) {
	// 	return osGeneralError;
	// }
	// todo: use version here

	if (floppy_configure(true, false, false, 8) != osOK) {
		return osGeneralError;
	}
	if (floppy_lock() != osOK) {
		return osGeneralError;
	}
	if (floppy_reset() != osOK) {
		return osGeneralError;
	}
	// configure all present drives
	FloppyDriveList drives;
	if (floppy_get_list(&drives) != osOK) {
		return osGeneralError;
	}

	for (i = 0; i < 4; i++) {
		if (drives.drives[i].exists == true) {
			if (floppy_recal(drives.drives[i], i) != osOK) {
				return osGeneralError;
			}
		}
	}

	return osOK;
}

osStatus floppy_simple_read (unsigned char drive, unsigned char sector, unsigned char track, unsigned char head, unsigned char count, char * destination) {
	int i;
	// make sure that it is selected and on.
	FloppyDriveList drives;
	if (floppy_get_list(&drives) != osOK) {
		return osGeneralError;
	}
	if (drives.drives[drive].exists != true) {
		return osGeneralError;
	}
	if (floppy_drive_select(&(drives.drives[drive]), drive, true) != osOK) {
		return osGeneralError;
	}
	// detect the motor
	bool motor = false;
	if (floppy_drive_get_motor(drive, &motor) != osOK) return osGeneralError;
	// make sure it's on
	if (!motor) {
		if (floppy_drive_set_motor(drive, true) != osOK) return osGeneralError;
	}
	// now, issue the seek command
	unsigned char st0;
	unsigned char pred_cylinder;
	for (i = 0; i < 3; i++) {
		if (floppy_seek(head, drive, track) != osOK) {
			return osGeneralError;
		}
		if (floppy_sense_interrupt(&st0, &pred_cylinder) != osOK) {
			return osGeneralError;
		}
		if (pred_cylinder == track) break;
		if (i == 2) return osGeneralError;
	}
	// initialise the DMA
	dma_init_floppy((unsigned int)destination, ((int)count * 512) - 1, 0x56);
	// issue the read command
	if (floppy_read(head, drive, track, sector, count, destination) != osOK)
		return osGeneralError;

	return osOK;
}
