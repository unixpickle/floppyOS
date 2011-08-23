TASK_OBJ_LEN     equ 76
TASK_LIST_BASE   equ 0x20000
TASK_COUNT       equ 0x509
TASK_CURRENT     equ 0x50D
TASK_DATA_SIZE   equ 0x10000
TASK_KERN_SIZE   equ 0x10000

global task_switch
task_switch:
	; expects a jump directly after a timer interrupt is called.
	mov ebx, TASK_COUNT
	mov eax, [ebx]
	cmp eax, 0
	jne savestate
	jmp taskswitch_cancel

savestate:
	; get current task
	mov ebx, TASK_CURRENT
	mov eax, [ebx]
	mov ecx, TASK_OBJ_LEN
	mov edx, 0
	imul ecx, eax
	; eax now contains the offset in the list
	mov ecx, TASK_LIST_BASE
	add eax, ecx
	; eax points to the task structure
	mov edi, eax
	add edi, 20
	mov esi, esp
	add esi, 4
	; backup esp
	mov eax, [esi+12]
	sub eax, 12
	mov [edi+12], eax
	mov edx, 8

copyRegisters32:
	; skip the esp register
	cmp edx, 3
	je skipRegister
	mov eax, [esi]
	mov [edi], eax
skipRegister:
	add esi, 4
	add edi, 4
	sub edx, 1
	cmp edx, 0
	je copyRegisters32done

copyRegisters32done:

	; TODO: subtrack the value of ESP by 4 quads.
	; or read from stack on PL switch

; now edi has ds, ss, gs, fs, es, cs

	; copy ds
	mov ax, [esp+2]
	mov [edi], ax
	; copy ss
	mov ax, [esp]
	mov [edi+2], ax
	; copy gs, fs, es
	mov ax, gs
	mov [edi+4], ax
	mov ax, fs
	mov [edi+6], ax
	mov ax, es
	mov [edi+8], ax
	; copy cs
	mov ax, [esp+40]
	mov [edi+10], ax
	; instruction pointer
	mov eax, [esp+36]
	mov [edi+12], eax
	; eflags
	mov eax, [esp+44]
	mov [edi+14], eax

	mov ax, [esp+40]
	; check for PL change
	and ax, 3
	cmp ax, 0
	je noPLChange
	; there was a PL change
	; copy old esp
	mov eax, [esp+48]
	mov [edi-20], eax
	; copy old ss
	mov ax, [esp+52]
	mov [edi+2], ax
noPLChange:
	
	; TODO: check for context switch.  If privilege level is the same,
	; simply subtrack the value of ESP by 3 quads. Otherwise, save the value
	; of the SS and ESP from the stack.

	; setup our new task
	call task_setcur
	; load LDT
	mov ebx, TASK_CURRENT

	; change TSS
	; setup stack for iret
	; perform task switching iret

taskswitch_cancel:
    pop bx
	mov ss, bx
	pop ax
	mov ds, ax

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

