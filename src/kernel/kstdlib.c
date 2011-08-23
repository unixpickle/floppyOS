#include "kstdlib.h"

void itoa (char * output, unsigned int input) {
	if (input == 0) {
		output[0] = '0';
		output[1] = 0;
		return;
	}
	char result[16];
	int cur = input;
	int resIndex;
	for (resIndex = 0; resIndex < 16; resIndex++) {
		result[resIndex] = 0;
	}
	resIndex = 0;
	while (cur != 0) {
		result[resIndex++] = 0x30 + (cur % 10);
		cur /= 10;
	}
	result[resIndex] = 0;
	cur = 0;
	while (resIndex > 0) {
		output[cur++] = result[--resIndex];
	}
	output[cur] = 0;
}

int strlen (const char * input) {
	int i = 0;
	while (input[i++] != 0) {}
	return i - 1;
}

void kdelay (int ms) {
	unsigned int waitUntil = kontime() + ms;
	while (kontime() < waitUntil) {
	}
}

void kmemcpy (char * dest, const char * source, int length) {
	int i;
	for (i = 0; i < length; i++) {
		dest[i] = source[i];
	}
}

