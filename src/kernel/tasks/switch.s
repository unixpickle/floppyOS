TASK_OBJ_LEN     equ 30
TASK_LIST_BASE   equ 0x200000
TASK_COUNT       equ 0x509
TASK_CURRENT     equ 0x50D
TASK_DATA_SIZE   equ 0x10000
TASK_KERN_SIZE   equ 0x10000
TASK_LOCK_PTR    equ 0x600

extern task_config
extern kprintnum
extern kprint

; expects a jump after a timer interrupt (or variant) is called.
global task_switch
task_switch:
	mov ebx, TASK_LOCK_PTR
	mov al, [ebx]
	cmp al, 0
	je taskswitch_begin
	jmp taskswitch_cancel

taskswitch_begin:
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
	mov ebx, eax
	
	; push other selectors
	mov ax, gs
	push ax
	mov ax, es
	push ax
	mov ax, fs
	push ax
	
	; save ESP
	mov [ebx+20], esp
	; set hasStarted to true
	mov [ebx+28], byte 1

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

	; check if it is NEEDED to load the new LDT.
	; if it isn't, we should re-load it.
	str ax
	cmp ax, 0x2b
	je skipLTR
	mov ax, 0x2b
	ltr ax

skipLTR:
	; check if the task is just getting started.
	mov al, [ebx+28]
	cmp al, 0
	je setupNewTask
	jmp reloadTask

setupNewTask:
	; push some magic stuff for the iret
	mov eax, 0xf
	push eax   ; old SS
	mov eax, 0xffff
	push eax   ; old ESP
	pushf      ; old eflags
	or [esp], dword 0x200
	mov eax, 0x7
	push eax   ; old CS
	mov eax, 0
	push eax   ; old EIP
	mov ax, 0xf
	mov ds, ax
	mov gs, ax
	mov fs, ax
	mov es, ax
	iret
	
reloadTask:
	mov eax, [ebx+20]
	mov esp, eax ; stack to restore from
	
	; pop everything in order
	pop ax
	mov fs, ax
	pop ax
	mov es, ax
	pop ax
	mov gs, ax
	; done before our control
	pop bx
	mov ss, bx
	pop ax
	mov ds, ax
	popad
	
	iret

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

