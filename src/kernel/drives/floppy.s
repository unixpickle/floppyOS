extern IO_wait

global floppy_read_register
floppy_read_register:
	;  ebp
	;  ...
	;  result
	;  registerNum
	;  eip
	;  esp
	push ebp
	mov ebp, esp

	mov ax, 0x3f0
	mov cx, [ebp+8]
	add ax, cx
	mov dx, ax
	in al, dx
	mov ebx, [ebp+12]
	mov [ebx], al

	mov esp, ebp
	pop ebp
	xor eax, eax
	ret

global floppy_write_register
floppy_write_register:
	;  ebp
	;  ...
	;  aByte
	;  registerNum
	;  eip
	;  esp
	push ebp
	mov ebp, esp

	mov ax, 0x3f0
	mov cx, [ebp+8]
	add ax, cx
	mov dx, ax
	mov eax, [ebp+12]
	out dx, al
	; wait for the IO operation to finish
	call IO_wait

	mov esp, ebp
	pop ebp
	xor eax, eax
	ret

global floppy_read_cmos
floppy_read_cmos:
	push ebp
	mov ebp, esp

	; disable interrupts so nothing else can break our CMOS...
	cli
	mov al, 0x10
	out 0x70, al
	call IO_wait
	xor eax, eax
	in al, 0x71
	sti

	; store destination byte
	mov ebx, [ebp+8]
	mov [ebx], al

	mov esp, ebp
	pop ebp
	xor eax, eax
	ret

