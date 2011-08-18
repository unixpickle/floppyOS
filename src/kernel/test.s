bits 32
org 0x1000

global _start
_start:
	mov ax,0x18						; Initialise gs to video memory
	mov gs,ax
	mov word [gs:0],0x0F41

	; 16 bit version
	;mov ax, 0xb800
	;mov es, ax
	;mov bx, 0
	;mov [es:bx], byte 66
	;mov [es:bx+1], byte 0x0F
spin: jmp spin

