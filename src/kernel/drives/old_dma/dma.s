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

	; set mask to DMA channel 2
	mov al, 0x6
	out 0xa, al

	; clear any existing data transfers
	mov al, 0
	out 0xc, al

	; write the mode to the DMA
	mov eax, [ebp+16]
	out 0xb, al

	; send the address (little endian)
	mov ax, [ebp+8]
	out 0x4, al
	mov al, ah
	out 0x4, al
	
	; send the page address
	mov ax, [ebp+10]
	out 0x81, al

	; send the count (pre-subtracted)
	mov ax, [ebp+12]
	out 0x5, al
	mov al, ah
	out 0x5, al

	; unmask DMA channel 2
	mov al, 0x02
	out 0xa, al

	sti
	mov esp, ebp
	pop ebp
	ret
