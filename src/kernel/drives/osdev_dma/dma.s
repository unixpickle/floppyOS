; before call
; [4 bytes] count
; [4 bytes] address
; [4 bytes] return address
global dma_init_floppy
dma_init_floppy:
	push ebp
	mov ebp, esp

	mov al, 0x06
	out 0x0a, al ; mask channel 2

	mov al, 0xff
	out 0x0c, al   ; reset flip-flop

	mov ecx, [ebp+8]
	mov al, cl
	out 0x04, al   ; address (low byte)
	mov al, ch
	out 0x04, al   ; address (high byte)

	mov al, 0xff
	out 0x0c, al   ; reset flip-flop

	mov ecx, [ebp+12]
	sub ecx, 1
	mov al, cl
	out 0x05, al   ; count (low byte)
	mov al, ch
	out 0x05, al   ; count (high byte)

	mov ax, [ebp+10]
	out 0x81, al   ; external page register

	mov al, 0x02
	out 0x0a, al   ; unmask DMA channel 2 only.

	mov esp, ebp
	pop ebp
	ret

global dma_prepare_read
dma_prepare_read:
	push ebp
	mov ebp, esp

	mov al, 0x06
	out 0x0a, al   ; mask DMA channel 2
	mov al, 0x5a
	out 0x0b, al   ; configure flags
	mov al, 0x02
	out 0x0a, al   ; unmask DMA channel 2

	mov esp, ebp
	pop ebp
	ret

global dma_prepare_write
dma_prepare_write:
	push ebp
	mov ebp, esp

	mov al, 0x06
	out 0x0a, al   ; mask DMA channel 2
	mov al, 0x56
	out 0x0b, al   ; configure flags
	mov al, 0x01
	out 0x0a, al   ; unmask DMA channel 2

	mov esp, ebp
	pop ebp
	ret
	
