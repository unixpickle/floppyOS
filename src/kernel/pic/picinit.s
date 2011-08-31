
PIC_EOI 	equ 	0x20
PIC2 		equ		0xA0
PIC1 		equ		0x20
PIC1_DATA	equ		PIC1+1
PIC2_DATA	equ		PIC2+1

; sends End-of-interrupt
; void PIC_sendEOI (unsigned char irq);
global PIC_sendEOI
PIC_sendEOI:
	push ebp
	mov ebp, esp

	mov ebx, esp
	mov al, [ebx+8]
	cmp al, 8
	jge PIC_sendEOI_2
	jmp PIC_sendEOI_cont
PIC_sendEOI_2:
	mov al, PIC_EOI
	out PIC2, al
PIC_sendEOI_cont:
	mov al, PIC_EOI
	out PIC1, al
	mov esp, ebp
	pop ebp
	ret

; writes a byte to an unused port to confirm the completion of I/O
; operations
global IO_wait
IO_wait:
	push ax
	mov al, 0
	out 0x80, al ; unused port...
	pop ax
	ret

; configures the PIC with interrupt numbers
; void PIC_remap (int offset1 /*master*/, int offset2);
; C version from http://wiki.osdev.org/PIC#Code_Examples
global PIC_remap
PIC_remap:
	push ebp
	mov ebp, esp
	sub esp, 2

	; al = PIC1-flags
	; ah = PIC2-flags
	in al, PIC2_DATA
	mov ah, al
	in al, PIC1_DATA
	push ax
	
	; provide commands for the PICs to start the
	; initialization sequence
	mov al, 0x11	; ICW1_INIT + ICW1_ICW4
	out PIC1, al
	call IO_wait
	mov al, 0x11	; ICW1_INIT + ICW1_ICW4
	out PIC2, al
	call IO_wait
	
	; give the PICs (PIC1 and PIC2) the offsets.
	; offsets start at [esp+12] or [ebp+8]
	mov ebx, ebp
	mov eax, [ebx+8]
	out PIC1_DATA, al
	call IO_wait
	mov ebx, ebp
	mov eax, [ebx+12]
	out PIC2_DATA, al
	call IO_wait

	; give "unknown" values...?
	mov al, 4
	out PIC1_DATA, al
	call IO_wait
	mov al, 2
	out PIC2_DATA, al
	call IO_wait

	; set PIC1 and PIC2 to ICW4 8086/88 (MCS-80/85) mode
	mov al, 0x01 ; ICW4_8086
	out PIC1_DATA, al
	call IO_wait
	mov al, 0x01 ; ICW4_8086
	out PIC2_DATA, al
	call IO_wait

	; write the initially saved flags
	; pop ax
	; out PIC1_DATA, al
	; mov al, ah
	; out PIC2_DATA, al
	mov al, 0
	out PIC1_DATA, al
	call IO_wait
	mov al, 0
	out PIC2_DATA, al
	sti

	mov esp, ebp
	pop ebp
	ret

