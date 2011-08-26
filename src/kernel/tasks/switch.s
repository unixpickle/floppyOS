TASK_OBJ_LEN     equ 76
TASK_LIST_BASE   equ 0x20000
TASK_COUNT       equ 0x509
TASK_CURRENT     equ 0x50D
TASK_DATA_SIZE   equ 0x10000
TASK_KERN_SIZE   equ 0x10000

extern task_config
extern kprintnum

global task_switch
task_switch:
	cli
	; expects a jump directly after a timer interrupt is called.
	mov ebx, TASK_COUNT
	mov eax, [ebx]
	cmp eax, 0
	jne check_current
	jmp taskswitch_cancel

check_current:
	; if current task is -1, then there is no current task.
	mov ebx, TASK_CURRENT
	mov eax, [ebx]
	cmp eax, 0xffffffff
	jne savestate
	jmp taskswitch_loadnew

savestate:
	; DEBUG: this prevents task switching from occuring
	; jmp taskswitch_cancel
	; get current task
	mov ebx, TASK_CURRENT
	mov eax, [ebx]
	mov ecx, TASK_OBJ_LEN
	mov edx, 0
	imul ecx
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
	mov ecx, 8

copyRegisters32:
	; skip the esp register
	cmp ecx, 3
	je skipRegister
	mov eax, [esi]
	mov [edi], eax
skipRegister:
	add esi, 4
	add edi, 4
	sub ecx, 1
	cmp ecx, 0
	je copyRegisters32done
	jmp copyRegisters32

copyRegisters32done:

; now edi has ds, ss, gs, fs, es, cs
	mov ebx, edi

	; copy ds
	mov ax, [esp+2]
	mov [ebx], ax
	; copy ss
	mov ax, [esp]
	mov [ebx+2], ax
	; copy gs, fs, es
	mov ax, gs
	mov [ebx+4], ax
	mov ax, fs
	mov [ebx+6], ax
	mov ax, es
	mov [ebx+8], ax
	; copy cs
	mov ax, [esp+40]
	mov [ebx+10], ax
	; instruction pointer
	mov eax, [esp+36]
	mov [ebx+12], eax
	
	; eflags
	mov eax, [esp+44]
	mov [ebx+16], eax

	mov ax, [esp+40]
	; check for PL change
	and ax, 3
	cmp ax, 0
	je noPLChange
	; there was a PL change
	; copy old esp
	mov eax, [esp+48]
	sub ebx, 20
	mov [ebx], eax
	; copy old ss
	mov ax, [esp+52]
	add ebx, 20
	mov [ebx+2], ax

noPLChange:

taskswitch_loadnew:
	; setup our new task
	call task_setcur

	; C subroutine for configuring the LDT segment descriptor
	; and TSS descriptor contents
	sgdt[0x51A]
	mov ebx, 0x51A
	mov eax, [ebx+2] ; skip two-byte limit field
	push eax
	call task_config
	add esp, 4
	mov ebx, eax 				; task_t pointer

	; change TSS and LDT
	mov ax, 0x20
	lldt ax
	
	str ax
	cmp ax, 0x2b
	je skipLTR
	mov ax, 0x2b
	ltr ax

skipLTR:

	; load up the GS, FS, ES registers
	;; 52 = ds
	;; 54 = ss
	;; 56 = gs
	;; 58 = fs
	;; 60 = es
	;; 62 = cs

	; test our LDT
	; mov ax, 0xf
	; mov fs, ax
	; jmp taskswitch_cancel
	mov gs, [ebx+56]
	mov fs, [ebx+58]
	mov es, [ebx+60]
	mov ax, [ebx+62]
	; setup stack for iret
	and ax, 3
	jz standardIRET
	; push special PL change stuff
	mov eax, 0
	mov ax, [ebx+54]
	push eax
	mov eax, [ebx+32]	; esp
	push eax			; old esp
	
standardIRET:
	;; 64 = eip
	;; 68 = eflags
	mov eax, [ebx+68]
	push eax
	or [esp], dword 0x200 ; set interrupt enable flag
	mov eax, 0
	; cs
	mov ax, [ebx+62]
	push eax
	; eip
	mov eax, [ebx+64]
	push eax

	; NOTE: stack currently configured for `iret`

	; All that's left is to restore the registers
	;; edi = 20
	;; esi = 24
	;; ebp = 28
	;; esp = 32
	;; ebx = 36
	;; edx = 40
	;; ecx = 44
	;; eax = 48
	mov edi, [ebx+20]
	mov esi, [ebx+24]
	mov ebp, [ebx+28]
	mov edx, [ebx+40]
	mov ecx, [ebx+44]
	; old eax
	mov eax, [ebx+48]
	push eax
	; old ebx
	mov eax, [ebx+36]
	push eax
	; old ds
	mov ax, [ebx+52]
	mov ds, ax
	; restore ebx and eax
	pop ebx
	pop eax
	
	iret
	; perform task switching iret

taskswitch_cancel:
    pop bx
	mov ss, bx
	pop ax
	mov ds, ax

	popad
	iret

task_setcur:
	mov ebx, TASK_COUNT
	mov ecx, [ebx]
	mov ebx, TASK_CURRENT
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

