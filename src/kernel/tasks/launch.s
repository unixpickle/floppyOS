; This file is legacy code that I used to test out how user-mode
; works.  It shouldn't be used commercially, just as a test.

extern kprint
extern katoi

global launchCodeAtPointer
launchCodeAtPointer:
	push ebp
	mov ebp, esp

	; LDT is stored at 0x8000
	mov eax, launchingLDT
	push eax
	call kprint
	pop eax
	
	mov edx, [ebp+8] ; requested code index
	mov bx, 0x8000
	mov ax, 0x10
	mov gs, ax
	
	; code selector
	mov ax, 0x1000 			; 4kb of code space should suffice
	mov [gs:bx], ax			; segment limit 15:00
	mov ax, 0x5000			; NOTE: change 0x5000 with dx
	mov [gs:bx+2], ax		; base address 15:00

	mov ax, 0				; lie about higher base address
	mov [gs:bx+4], al		; base address 23:16
	mov al, 0xfa			; code segment (privilege level 3)
	mov [gs:bx+5], al
	mov al, 0xcf			; AVL, 0, D, G
	mov [gs:bx+6], al
	mov [gs:bx+7], ah		; base address 31:24

	; data selector
	mov edx, 0x6000			; preset base data address
	add bx, 8
	mov ax, 0x1000 			; 4kb of data space
	mov [gs:bx], ax			; segment limit 15:00
	mov ax, dx
	mov [gs:bx+2], ax		; base address 15:00
	mov ax, 0				; lie about higher base address
	mov [gs:bx+4], al		; base address 23:16
	mov al, 0xf2			; data segment (should be 0xf2)
	mov [gs:bx+5], al
	mov al, 0xcf			; AVL, 0, D, G
	mov [gs:bx+6], al
	mov [gs:bx+7], ah		; base address 31:24


	;add bx, 8
	;mov ax, 0xffff
	;mov [gs:bx], ax
	;mov ax, 0x6000
	;mov [gs:bx+2], ax
	;mov [gs:bx+4], byte 0
	;mov [gs:bx+5], byte 0x92
	;mov [gs:bx+6], byte 0x4f
	;mov [gs:bx+7], byte 0
	
	; at this point our TSS segment points to 0x9000
	; now, we will bzero it!
	mov ebx, 0x9000
	mov ax, 104
zerotss_loop:
	mov [ebx], byte 0
	dec ax
	inc ebx
	cmp ax, 0
	jne zerotss_loop
	
	; configure the TSS
	call configureTSS
	
	; load the ldt
	mov ax, 0x20
	lldt ax

	; load the task state segment
	mov ax, 0x2b
	ltr ax

	; display OK
	mov eax, okMsg
	push eax
	call kprint
	pop eax

	mov eax, configuringSegments
	push eax
	call kprint
	pop eax

	cli
	mov eax, esp
	push dword 0xf
	push dword 0x1000
	pushfd
	or [esp], dword 0x200
	push dword 0x07
	push dword 0x00
	
	mov ax, 0xf
	mov ds, ax
	mov gs, ax
	mov es, ax
	iret
	
	; cause fault
	;mov ax, 0xf
	;mov ss, ax
	;mov ebx, 0
	;push long 0x0
	;push long 0x0
	;push long 0x0
	;push long 0x0
	;push long 0x0
	;push long 0x0
	
	jmp $
	;jmp 0x7:0x0				; descriptor 0, LDT, user level
	;mov ebx, 0x10
	;mov [ebx], byte 0x68
	;jmp 0x5000
	
	;push ax
	;pop ax
	;spinloop: jmp $

	mov esp, ebp
	pop ebp
	ret

; sets the TSS with a kernel stack pointer
configureTSS:
	push ebp
	mov ebp, esp

	mov ebx, 0x9004
	mov [ebx], dword esp	; set esp0
	mov ebx, 0x9008
	mov [ebx], word 0x10	; set ss0
	mov ebx, 0x904c
	mov [ebx], word 0x0b	; set cs
	mov ebx, 0x9050
	mov ax, 0x13
	mov [ebx], ax			; set ss
	mov [ebx+4], ax			; set ds
	mov [ebx+8], ax			; set fs
	mov [ebx+4], ax			; set gs
	mov [ebx-8], ax			; set es
	
	mov ebx, 0x9060
	mov ax, 0x20
	mov [ebx], ax			; set ldt

	mov esp, ebp
	pop ebp
	ret

launchingLDT: 			db 'Loading LDT ... ',0
okMsg:					db '[OK]',10,0
configuringSegments:	db 'Configuring segments ... ',0

