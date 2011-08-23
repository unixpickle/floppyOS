global task_switch
task_switch:
	; expects a jump directly after a timer interrupt is called.
	mov ebx, 0x509
	mov eax, [ebx];
	cmp eax, 0
	je taskswitch_cancel

	; save current task state (including registers pushed to the stack).

	; setup our new task
	call task_setcur
	; load LDT
	; change TSS
	; setup stack for iret
	; perform task switching iret

taskswitch_cancel:
	popad
	iret

task_setcur:
	mov ebx, 0x509
	mov ecx, [ebx]
	mov ebx, 0x50D
	mov eax, [ebx]
	add eax, 1
	cmp eax, ecx
	jge wrapZero    ; wrap back to zero
	jmp setValue
wrapZero:
	mov eax, 0
setValue:
	mov [ebx], eax
	ret

