#include "lock.h"

extern void kprint (const char * aStr);

void lock_buff_reset () {
	int i;
	unsigned char * lockBase = kLockInfoBase;
	for (i = 0; i < 34; i++) {
		lockBase[i] = 0;
	}
}

void lock_set_taskswitch (bool flag) {
	unsigned char * lockBase = kLockInfoBase;
	lockBase[1] = flag;
}

bool lock_get_taskswitch () {
	unsigned char * lockBase = kLockInfoBase;
	return (bool)(lockBase[1]);
}

void lock_cpu () {
	unsigned char * lockBase = kLockInfoBase;
	lockBase[0] += 1;
}

void unlock_cpu () {
	unsigned char * lockBase = kLockInfoBase;
	lockBase[0] -= 1;
}

void lock_vector (int index) {
	unsigned char * lockBase = kLockInfoBase;
	lock_cpu();
	if (index < 1 || index > 32) {
		unlock_cpu();
		return;
	}
	if (lockBase[index + 1] != 0) {
		while (lockBase[index + 1] != 0) {
			unlock_cpu();
			if (lock_get_taskswitch()) {
				// asm("int $0x81"); // swap out if needed
				asm("hlt");
			}
			lock_cpu();
		}
	}
	// CPU is still locked; make this quick
	lockBase[index + 1] = 1;
	unlock_cpu();
}

void unlock_vector (int index) {
	unsigned char * lockBase = kLockInfoBase;
	lock_cpu();
	if (index < 1 || index > 32) {
		unlock_cpu();
		return;
	}
	lockBase[index + 1] = 0;
	// trigger a task switch to resign control.
	unlock_cpu();
	if (lock_get_taskswitch()) {
		// kprint("Not unlocking switch");
		asm("int $0x81");
		// kprint("Done switch!");
	}
}

