#include <kernel/kpi.h>
#include <kernel/kstdlib.h>

typedef enum {
	KeyboardShift = 1,
	KeyboardAlt = 2,
	KeyboardCtrl = 3
} KeyboardModifier;

unsigned char keyboard_get_modifiers ();
void keyboard_set_modifiers (unsigned char modifiers);
void keyboard_handle_scan (unsigned char scanCode);
char keyboard_wait_key ();

