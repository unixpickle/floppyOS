#include <kernel/kpi.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt/idtload.h>
#include <kernel/idt/idtinit.h>
#include <kernel/pic/picinit.h>
#include <kernel/tasks/launch.h>
#include <kernel/drives/simplefloppy.h>

void kprintok ();
void kprinterr ();
void kerror (const char * msg);

void kmain () {
	int i;

	kprint("Setting on time to 0 ... ");
	ksettime(0);
	kprintok();
	kprint("Loading kernel ... [OK]\n");
	kprint("Configuring IDT ... ");
	configureIDT();
	kprintok();
	kprint("Loading IDT ... ");
	loadIDT();
	kprintok();

	kprint("Initialising PICs ... ");
	/* interrupt 32-47 will be for IRQ0-15 */
	PIC_remap(32, 40);
	kprintok();

	kprint("Configuring PIT ... ");
	configurePIT();
	kprintok();

	FloppyDriveList drives;
	if (floppy_get_list(&drives) != osOK) {
		kerror("Failed to get list of drives!\n");
	}
	for (i = 0; i < 4; i++) {
		char aStr[32];
		itoa(aStr, i);
		kprint("FloppyDrive #");
		kprint(aStr);
		kprint(" = ");
		floppy_get_string(aStr, &(drives.drives[i]));
		kprint(aStr);
		kprint("\n");
	}

	unsigned char * myBuf = (char *)0x8000;
	kprint("Floppy setup... ");
	if (floppy_setup() != osOK) {
		kprinterr();
	}
	kprintok();
	kprint("Floppy read... ");
	// sector 1, drive 0, track 0, head 0, count 1, etc.
	if (floppy_simple_read(0, 1, 0, 0, 1, myBuf) != osOK) {
		kprinterr();
	}
	kprintok();
	kprint("Printing buffer...\n");
	kdelay(500);
	int p;
	for (p = 0; p < 512; p++) {
		char foo[2];
		foo[0] = myBuf[p];
		foo[1] = 0;
		kprint(foo);
		kdelay(40);
	}
	kprintok();

	while (1) { }
}

void khandle_key (unsigned int scanCode) {
	char str[4];
	/*char str[16];
	itoa(str, (int)scanCode);
	kprint("Scan code: ");
	kprint(str);
	kprint("\n");
	return;*/
	char scan = scan_code_to_ascii(scanCode);
	if (scan) {
		str[0] = scan;
		str[1] = 0;
		kprint(str);
	} else if ((scanCode & 0x80) == 0) {
		kprint("?");
	}
}

void kprintok () {
	kprint("[OK]\n");
}

void kprinterr () {
	kerror("[FAILED]\n");
}

void kerror (const char * msg) {
	kprint(msg);
	while (1) { }
}

