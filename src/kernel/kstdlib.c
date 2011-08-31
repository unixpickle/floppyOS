#include "kstdlib.h"

void itoa (char * output, unsigned int input) {
	if (input == 0) {
		output[0] = '0';
		output[1] = 0;
		return;
	}
	char result[16];
	unsigned int cur = input;
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

void itohs (char * buffer, unsigned int hexNum) {
	// TODO: magic happens here.
	int i, digNum;
	unsigned int theNum = hexNum;
	bool hasStarted = false;
	unsigned char * buff = (unsigned char *)&theNum;
	for (i = 0; i < 16; i++) {
		buffer[i] = 0;
	}
	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[2] = '0';
	i = 2;
	digNum = 7;
	while (digNum >= 0 && digNum < 8) {
		unsigned int num = 0; // will be 0 to 15
		char aChar = 0;
		unsigned int charIndex = digNum / 2;
		if (digNum % 2 == 0) {
			num = buff[charIndex] & 0xf;
		} else {
			num = (buff[charIndex] >> 4) & 0xf;
		}
		if (num != 0 || hasStarted) {
			hasStarted = true;
			if (num < 10) {
				aChar = num + '0';
			} else {
				aChar = (num - 10) + 'A';
			}
			buffer[i++] = aChar;
		}
		digNum -= 1;
	}
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

