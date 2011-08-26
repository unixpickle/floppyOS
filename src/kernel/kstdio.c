#include "kpi.h"
#include <kernel/tasks/lock.h>

void kprint (const char * string) {
	// lock_vector(kPrintLockNum);
	int i;
	char c = 0;
	i = 0;
	while ((c = string[i++]) != 0) {
		unsigned int cursor = kcurpos();
		if (string[i-1] == '\n') {
			int modInt = cursor % 80;
			cursor += (80 - modInt);
		} else {
			kputc(c);
			cursor += 1;
		}
		while (cursor >= 2000) {
			kscroll();
			cursor -= 80;
		}
		kcurmove((unsigned short)cursor);
	}
	// unlock_vector(kPrintLockNum);
}

extern void itoa (char * dest, unsigned int aNum);

void kprintnum (unsigned int aNumber) {
	char dest[16];
	itoa(dest, aNumber);
	kprint(dest);
	kprint("\n");
}

