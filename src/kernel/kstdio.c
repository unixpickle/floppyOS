#include "kpi.h"

void kprint (const char * string) {
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
}
