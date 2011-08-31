; Input
; ax = reload value (11931 for 100hz)

global init_PIT
init_PIT:
	pushfd
	cli
	out 0x40, al
	rol ax, 8
	out 0x40, al
	rol ax, 8
	popfd
	ret

