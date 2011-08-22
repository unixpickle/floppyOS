#include "keyboard.h"

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
				ascii -= ((unsigned char)'a' - (unsigned char)'A');
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

