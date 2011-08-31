
extern mainTable
extern init_PIT

global loadIDT
loadIDT:
	lidt[mainTable]
	ret

global configurePIT
configurePIT:
	; 8000hz is our frequency
	mov eax, 11931
	call init_PIT
	ret
