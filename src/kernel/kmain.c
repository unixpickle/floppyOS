#include <kernel/kpi.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt/idtload.h>
#include <kernel/idt/idtinit.h>
#include <kernel/pic/picinit.h>
#include <kernel/drives/simplefloppy.h>
#include <kernel/keyboard.h>
#include <kernel/tasks/tasks.h>

void kprintok ();
void kprinterr ();
void kerror (const char * msg);

void kmain () {
	int i;

	task_list_reset();

	kprint("Setting on time to 0 ... ");
	ksettime(0);
	kprintok();
	kprint("Resetting keyboard ... ");
	keyboard_set_modifiers(0);
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

	unsigned char * myBuf = (char *)0x5000;
	kprint("Floppy setup ... ");
	if (floppy_setup() != osOK) {
		kprinterr();
	}
	kprintok();

	kprint("Floppy read ... ");
	// sector 3, drive 0, track 0, head 0, count 1, etc.
	if (floppy_simple_read(0, 3, 0, 0, 1, myBuf) != osOK) {
		kprinterr();
	}
	kprintok();

/*
	kprint("Printing buffer ...\n");
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
*/

	asm("cli");
	kprint("Resetting task space ... ");
	task_list_reset();
	kprintok();
	kprint("Starting test program ... ");
	pid_t pid;
	if (task_start(myBuf, 512, &pid) != osOK) {
		kprinterr();
	}
	kprintok();
	asm("sti");

	while (1) {
		char kbuf[2];
		kbuf[1] = 0;
		kbuf[0] = keyboard_wait_key ();
		kprint(kbuf);
	}
}

void khandle_key (unsigned int scanCode) {
	keyboard_handle_scan(scanCode);
	/*
	char str[16];
	itoa(str, (int)scanCode);
	kprint("Scan code: ");
	kprint(str);
	kprint("\n");
	*/
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
