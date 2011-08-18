BITS 32

extern IO_wait

DIGITAL_OUTPUT_REGISTER equ 0x3f2
MAIN_STATUS_REGISTER equ 0x3f4
DATA_FIFO equ 0x3f5
DATARATE_SELECT_REGISTER equ 0x3f4
CONFIGURATION_CONTROL_REGISTER equ 0x3f7

global floppy_read_cmos
floppy_read_cmos:
	push ebp
	mov ebp, esp

	cli
	mov al, 0x10
	out 0x70, al
	call IO_wait
	xor eax, eax
	in al, 0x71
	sti

	mov esp, ebp
	pop ebp
	ret

global floppy_needs_reset
floppy_needs_reset:
	push ebp
	mov ebp, esp

	mov dx, MAIN_STATUS_REGISTER
	in al, dx
	and al, 0xc0
	cmp al, 0x80
	jne noReset
	mov eax, 1
	jmp floppy_needs_reset_return
noReset:
	mov eax, 0
floppy_needs_reset_return:
	mov esp, ebp
	pop ebp
	ret

global floppy_read_FIFO
floppy_read_FIFO:
	push ebp
	mov ebp, esp

	xor eax, eax
	mov dx, DATA_FIFO
	in al, dx

	mov esp, ebp
	pop ebp
	ret

global floppy_write_FIFO
floppy_write_FIFO:
	push ebp
	mov ebp, esp

	mov eax, [ebp+8]
	mov dx, DATA_FIFO
	out dx, al
	call IO_wait

	mov esp, ebp
	pop ebp
	ret

global floppy_read_DOR
floppy_read_DOR:
	push ebp
	mov ebp, esp

	xor eax, eax
	mov dx, DIGITAL_OUTPUT_REGISTER
	in al, dx

	mov esp, ebp
	pop ebp
	ret

global floppy_write_DOR
floppy_write_DOR:
	push ebp
	mov ebp, esp

	mov eax, [ebp+8]
	mov dx, DIGITAL_OUTPUT_REGISTER
	out dx, al
	call IO_wait

	mov esp, ebp
	pop ebp
	ret

global floppy_write_DSR
floppy_write_DSR:
	push ebp
	mov ebp, esp

	mov eax, [ebp+8]
	mov dx, DATARATE_SELECT_REGISTER
	out dx, al
	call IO_wait

	mov esp, ebp
	pop ebp
	ret

global floppy_write_CCR
floppy_write_CCR:
	push ebp
	mov ebp, esp

	mov eax, [ebp+8]
	mov dx, CONFIGURATION_CONTROL_REGISTER
	out dx, al
	call IO_wait

	mov esp, ebp
	pop ebp
	ret

global floppy_read_MSR
floppy_read_MSR:
	push ebp
	mov ebp, esp

	xor eax, eax
	mov dx, MAIN_STATUS_REGISTER
	in al, dx

	mov esp, ebp
	pop ebp
	ret

