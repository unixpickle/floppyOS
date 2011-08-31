#include <kernel/kstdlib.h>

#define kLockInfoBase (unsigned char *)0x600
#define kPrintLockNum 1
#define kFloppyLockNum 2

void lock_buff_reset ();

void lock_set_taskswitch (bool flag);
bool lock_get_taskswitch ();

/**
 * Prevent timer task switching.
 */
void lock_cpu ();

/**
 * Allow timer task switching.
 */
void unlock_cpu ();

/**
 * Locks a lock at an index.
 * @param index An index pointing to the lock vector.
 * Minimum value is 1, maximum is 32.
 */
void lock_vector (int index);

/**
 * Unlocks a lock at an index.
 * @param index An index pointing to the lock vector.
 * Minimum value is 1, maximum is 32
 */
void unlock_vector (int index);

