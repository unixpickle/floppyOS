BITS 32

extern kmain
global kentry
kentry:
	push ebp
	mov ebp, esp

	call kmain

	mov esp, ebp
	pop ebp
	ret
