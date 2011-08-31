kPrintLockNum equ 1

extern kprint
extern kprintnum
extern PIC_sendEOI
extern khandle_key
extern lock_vector
extern unlock_vector

%macro systemSelectors 0

	mov ax, ds
	push ax
	mov bx, ss
	push bx
	mov ax, 0x10
	mov ds, ax
	mov ss, ax

%endmacro

%macro restSystemSelectors 0

	pop bx
	mov ss, bx
	pop ax
	mov ds, ax

%endmacro

extern handleFloppyCallback
global floppyCallback
floppyCallback:
	pushad
	systemSelectors
	call handleFloppyCallback
	mov eax, 7
	push eax
	call PIC_sendEOI
	pop eax
	restSystemSelectors
	popad
	iret

global handleMathException
handleMathException:
	pushad
	systemSelectors

	mov eax, [esp+20]
	; calculate task address
	
	push eax
	call kprint
	add esp, 4

	restSystemSelectors
	popad
	iret

extern task_translate_addr
global handleSysCall
handleSysCall:
	pushad
	systemSelectors
	
	mov eax, kPrintLockNum
	push eax
	call lock_vector
	add esp, 4

	mov eax, [esp+20]

	push eax
	call task_translate_addr
	add esp, 4

	push eax
	call kprint
	add esp, 4

	mov eax, kPrintLockNum
	push eax
	call unlock_vector
	add esp, 4
	; int 0x81

	restSystemSelectors
	popad
	iret

mathExceptionMsg: db 'Exception: Divide by zero',10,0

global handleInvalidOpcode
handleInvalidOpcode:
	pushad
	mov eax, invalidOpcodeException
	push eax
	call kprint
	pop eax
	popad
spinForever:
	jmp spinForever
	iret

invalidOpcodeException: db 'Exception: invalid opcode',10,0

global handleUnknownException
handleUnknownException:
	pushad
	mov eax, unknownMsg
	push eax
	call kprint
	pop eax
	popad
	iret

unknownMsg: db 'Exception: unknown cause',10,0

global handleGPFault
handleGPFault:
	cli
	pushad
	systemSelectors

	mov eax, gpfException
	push eax
	call kprint
	add esp, 4

gpspin: jmp $
	sti

	restSystemSelectors
	popad
	add esp, 4 ; error code
	iret

gpfException:			db 'Exception: general protection fault',10,0

extern kaddtime

extern task_switch
global handleHardwareTimer
handleHardwareTimer:
	cli
	pushad
	systemSelectors
	
	; add 10 ms to system on time.
	mov eax, 10
	push eax
	call kaddtime
	pop eax

	mov eax, 1
	push eax
	call PIC_sendEOI
	pop eax

	; will soon be used for multitasking.
	jmp task_switch

global manualTaskSwitch
manualTaskSwitch:
	cli
	pushad
	systemSelectors

	jmp task_switch

global keyPress
keyPress:
	cli
	pushad

waitScan:
	in al, 0x64
	and al, 0x01
	jz waitScan

	xor eax, eax
	in al, 0x60
	push eax
	call khandle_key
	pop eax

	mov eax, 2
	push eax
	call PIC_sendEOI
	pop eax

	popad
	iret

