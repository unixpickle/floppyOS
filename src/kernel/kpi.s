BITS 32

extern IO_wait

global kputc
kputc:
	push ebp
	mov ebp, esp

	; write 14 to the output port to specify that we want the higher byte
	mov dx, 0x3D4
	mov al, 14
	out dx, al
	mov dx, 0x3D5
	in al, dx
	call IO_wait
	mov ah, al
	; write 15 to the output port to specify that we want the lower byte
	mov dx, 0x3D4
	mov al, 15
	out dx, al
	call IO_wait
	mov dx, 0x3D5
	in al, dx
	call IO_wait
	
	mov ebx, esp
	mov edx, [ebx+8] ; the argument (char)
	push edx

	mov bx, 2
	mov dx, 0
	mul bx
	mov bx, ax

	; load gs to video selector (0x18)
	pop edx
	mov ax, 0x18
	mov gs, ax
	mov [gs:bx], dl
	mov [gs:bx+1], byte 0x0F

	mov esp, ebp
	pop ebp
	ret

global kcurpos
kcurpos:
	push ebp
	mov ebp, esp
	
	mov eax, 0

	; write 14 to the output port to specify that we want the higher byte
	mov dx, 0x3D4
	mov al, 14
	out dx, al
	call IO_wait
	mov dx, 0x3D5
	in al, dx
	mov ah, al
	; write 15 to the output port to specify that we want the lower byte
	mov dx, 0x3D4
	mov al, 15
	out dx, al
	call IO_wait
	mov dx, 0x3D5
	in al, dx

	mov esp, ebp
	pop ebp
	ret

global kcurmove

kcurmove:
	push ebp
	mov ebp, esp

	mov ebx, esp
	mov cx, [ebx+8]
	
	mov dx, 0x3D4
	mov al, 14
	out dx, al
	call IO_wait
	mov cx, [ebx+8]
	mov dx, 0x3D5
	mov al, ch
	out dx, al
	call IO_wait
	
	mov dx, 0x3D4
	mov al, 15
	out dx, al
	call IO_wait
	mov cx, [ebx+8]
	mov dx, 0x3D5
	mov al, cl
	out dx, al
	call IO_wait

	mov esp, ebp
	pop ebp
	ret

global kscroll
kscroll:
	push ebp
	mov ebp, esp

	mov ax, 0x18
	mov gs, ax
	mov bx, 0
	; bx = byte index
kscroll_loop:
	cmp bx, 3840
	je kscroll_loopd
	mov cx, bx
	add cx, 160
	push bx
	mov bx, cx
	mov ax, [gs:bx]
	pop bx
	mov [gs:bx], ax
	add bx, 2
	jmp kscroll_loop

kscroll_loopd:
	mov bx, 3840
kscroll_cloop:
	cmp bx, 4000
	je kscroll_done
	mov al, 0
	mov [gs:bx], al
	mov [gs:bx+1], byte 0x07
	add bx, 2
	jmp kscroll_cloop
kscroll_done:
	mov esp, ebp
	pop ebp
	ret

global kgettime
kgettime:
	push ebp
	mov ebp, esp

	; read the magical 

	mov esp, ebp
	pop ebp
	ret


global kontime
kontime:
	push ebx
	mov ebx, 0x500
	mov eax, [ebx]
	pop ebx
	ret

global kaddtime
kaddtime:
	push ebx
	push ecx
	mov ebx, 0x500
	mov eax, [ebx]
	mov ecx, [esp+12]
	add eax, ecx
	mov [ebx], eax
	pop ecx
	pop ebx
	xor eax, eax
	ret

global ksettime
ksettime:
	push ecx
	push ebx
	mov ebx, 0x500
	mov ecx, [esp+12]
	mov [ebx], ecx
	pop ebx
	pop ecx
	ret

