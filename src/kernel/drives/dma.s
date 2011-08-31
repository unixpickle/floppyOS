extern IO_wait

; before call
; [4 bytes] mode
; [4 bytes] count
; [4 bytes] address
; [4 bytes] return address
global dma_init_floppy
dma_init_floppy:
	push ebp
	mov ebp, esp
	cli

	; mask DMA channel 2
	mov al, 0x6
	out 0xa, al
	; reset master flip-flop
	mov al, 0xff
	out 0xc, al
	; address low-high
	mov ax, [ebp+8]
	out 0x4, al
	mov al, ah
	out 0x4, al
	; reset master flip-flop
	mov al, 0xff
	out 0xc, al
	; send the count low-high
	mov eax, [ebp+12]
	out 0x5, al
	mov al, ah
	out 0x5, al
	; send the page register
	mov al, 0x0
	out 0x81, al ; page register
	; send the mode
	mov eax, [ebp+16]
	out 0xb, al
	; unmask DMA channel 2
	mov al, 0x2
	out 0xa, al

	sti
	mov esp, ebp
	pop ebp
	ret
