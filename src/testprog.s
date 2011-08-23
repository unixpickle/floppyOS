; This will be the program loaded from the 3rd sector of the floppy and executed
; to test the task system

BITS 32
org 0x0

_start:
	int 0x80
myLoop:
	jmp myLoop
