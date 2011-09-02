; This will be the program loaded from the 3rd sector of the floppy and executed
; to test the task system

BITS 32
org 0x0

_start:
	mov ebx, coolString
	int 0x80
spin:
	; makes other tasks get more CPU time
	; by co-operating with the kernel
	; int 0x81
	jmp _start
	; jmp spin

coolString: db 'Hello, task world!',10,0
