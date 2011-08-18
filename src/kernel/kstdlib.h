#ifndef KSTDLIB_H
#define KSTDLIB_H

typedef unsigned char bool;
#define true 1
#define false 0

typedef unsigned short osStatus;
#define osOK 0
#define osGeneralError -1
#define osTimeoutError -2

/**
 * convert a number to a null terminated string
 * @param output The output string (should be 16 characters long at least)
 * @param input The number to convert
 */
void itoa (char * output, unsigned int input);

/**
 * returns the length of a null terminated string.
 */
int strlen (const char * input);

/**
 * Waits a certain number of milliseconds.
 */
void kdelay (int ms);

#endif
