#include "keyboard.h"

static char scan_code_to_ascii (unsigned char scanCode);
static char char_apply_shift (unsigned char ascii);

unsigned char keyboard_get_modifiers () {
	unsigned char * kbPtr = (unsigned char *)0x506;
	return *kbPtr;
}

void keyboard_set_modifiers (unsigned char modifiers) {
	unsigned char * kbPtr = (unsigned char *)0x506;
	*kbPtr = modifiers;
}

void keyboard_handle_scan (unsigned char scanCode) {
	unsigned char upModifier = (scanCode & 0x80) >> 7;
	unsigned char regKeycode = (scanCode & (0xff ^ 0x80));
	if (regKeycode == 42 || regKeycode == 54) {
		if (upModifier == 0) {
			keyboard_set_modifiers(keyboard_get_modifiers() | KeyboardShift);
		} else {
			unsigned char mask = (0xff ^ KeyboardShift);
			keyboard_set_modifiers(keyboard_get_modifiers() & mask);
		}
	} else if (!upModifier) {
		char ascii = scan_code_to_ascii(regKeycode);
		if (ascii) {
			if ((keyboard_get_modifiers() & KeyboardShift) != 0) {
				ascii = char_apply_shift((unsigned char)ascii);
			}
			unsigned char * lastKey = (unsigned char *)0x507;
			*lastKey = ascii;
			lastKey[1] += 1; // index ++
		}
	}
}

char keyboard_wait_key () {
	unsigned char * lastKey = (unsigned char *)0x507;
	unsigned char lastIndex = lastKey[1];
	while (lastIndex == lastKey[1]) {
		asm("hlt");
	}
	return (char)*lastKey;
}

static char scan_code_to_ascii (unsigned char scanCode) {
	if (scanCode >= 16 && scanCode <= 25) {
		const char * chars = "qwertyuiop";
		return chars[scanCode - 16];
	} else if (scanCode >= 30 && scanCode <= 38) {
		const char * chars = "asdfghjkl";
		return chars[scanCode - 30];
	} else if (scanCode >= 44 && scanCode <= 50) {
		const char * chars = "zxcvbnm";
		return chars[scanCode - 44];
	} else if (scanCode == 57) {
		return ' ';
	} else if (scanCode > 1 && scanCode < 11) {
		return (scanCode - 2) + '1';
	} else if (scanCode == 11) {
		return '0';
	} else if (scanCode == 28) {
		return '\n';
	} else if (scanCode == 14) {
		return 8;
	}
	return 0;
}

static char char_apply_shift (unsigned char ascii) {
	if (ascii >= 'a' && ascii <= 'z') {
		return ascii - ((unsigned char)'a' - (unsigned char)'A');
	} else if (ascii >= '0' && ascii <= '9') {
		char asciiChars[] = ")!@#$%^&*(";
		return asciiChars[ascii - '0'];
	}
	return ascii;
}

