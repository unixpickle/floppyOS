#include "interrupt.h"

struct IDTDesc {
	unsigned short offset_low;  // low offset
	unsigned short selector;    // code segment
	unsigned char zero;
	unsigned char type_attr;    // access byte
							    // values: 0x8e 0x8f 0xee 0xef
	unsigned short offset_high; // high offset
} __attribute__((__packed__));

struct IDTable {
	unsigned short limit;
	unsigned int base;
	struct IDTDesc entries[256];
} __attribute__((__packed__));

struct IDTable mainTable;
	
void setIDT (int index, unsigned int offset, unsigned short codesegment, unsigned char attributes) {
	struct IDTDesc * descriptor = &(mainTable.entries[index]);
	descriptor->offset_low = (unsigned short)(offset & 0xFFFF);
	descriptor->selector = codesegment;
	descriptor->zero = 0;
	descriptor->type_attr = attributes;
	descriptor->offset_high = (unsigned short)((offset >> 16) & 0xFFFF);
}

void configureIDT () {
	int i;
	for (i = 0; i < 256*sizeof(struct IDTDesc); i++) {
		((char *)(&mainTable.entries))[i] = 0;
	}
	// load IDTs here
	// 0x8e = interrupt gate
	setIDT(0, (unsigned)handleMathException, 0x08, 0x8e);
	for (i = 1; i < 6; i++) {
		setIDT(i, (unsigned)handleUnknownException, 0x08, 0x8e);
	}
	setIDT(6, (unsigned)handleInvalidOpcode, 0x08, 0x8e);
	for (i = 7; i < 0x0D; i++) {
		setIDT(i, (unsigned)handleUnknownException, 0x08, 0x8e);
	}
	setIDT(0x0D, (unsigned)handleGPFault, 0x08, 0x8e);
	setIDT(0x0E, (unsigned)handleUnknownException, 0x08, 0x8e);
	setIDT(0x0F, (unsigned)handleUnknownException, 0x08, 0x8e);
	setIDT(32, (unsigned)handleHardwareTimer, 0x08, 0x8e);
	setIDT(33, (unsigned)keyPress, 0x08, 0x8e);
	setIDT(38, (unsigned)floppyCallback, 0x08, 0xee);
	setIDT(0x80, (unsigned)handleSysCall, 0x08, 0xee);
	mainTable.limit = 2048;
	mainTable.base = (unsigned)&(mainTable.entries);
}

