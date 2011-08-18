#ifndef FLOPPYTYPES_H
#define FLOPPYTYPES_H

#include <kernel/kstdlib.h>

struct _FloppyDrive {
	unsigned char sectorsPerTrack;
	unsigned char tracks;
	unsigned char heads;
	unsigned short motorDelay;
	unsigned char cmosType;
	unsigned char datarate; // 0 = 500Kbps, 3 = 1Mbps (2.88M)
	bool exists;
	bool supported;
} __attribute__((__packed__));

typedef struct _FloppyDrive FloppyDrive;

typedef struct {
	FloppyDrive drives[4];
} FloppyDriveList;

typedef enum {
	FDC_CMD_READ_TRACK = 2,
	FDC_CMD_SPECIFY = 3,
	FDC_CMD_SENSE_DRIVE_STATUS = 4,
	FDC_CMD_WRITE_DATA = 5,
	FDC_CMD_READ_DATA = 6,
	FDC_CMD_RECALIBRATE = 7,
	FDC_CMD_SENSE_INTERRUPT = 8,
	FDC_CMD_WRITE_DELETED_DATA = 9,
	FDC_CMD_READ_ID = 10,
	FDC_CMD_READ_DELETED_DATA = 12,
	FDC_CMD_READ_FORMAT_TRACK = 13,
	FDC_CMD_SEEK = 15,
	FDC_CMD_VERSION = 16,
	FDC_CMD_SCAN_EQUAL = 17,
	FDC_CMD_PERPENDICULAR_MODE = 18,
	FDC_CMD_CONFIGURE = 19,
	FDC_CMD_LOCK = 20,
	FDC_CMD_VERIFY = 22,
	FDC_CMD_SCAN_LOW_OR_EQUAL = 25,
	FDC_CMD_SCAN_HIGH_OR_EQUAL = 29,
} FloppyCommand;

#endif
