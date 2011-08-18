BITS 32
org 0x5000

_start:
	int 0x80
myLoop:
	jmp myLoop
