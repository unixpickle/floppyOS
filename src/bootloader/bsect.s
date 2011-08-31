org 0x7c40				 ; Start address (plus 64 byte offset)

bits 16
_start:
	; load the kernel from the floppy
	mov ax, 0x500
    mov es, ax
    mov bx, 0 

    mov dl, 0 
    mov dh, 0 
    mov ch, 0 
    mov cl, 3 ; sector 3
    mov al, 1 ; 1 sector

    mov ah, 2

    int 0x13
	
	; load test program from the floppy
	mov ax, 0x100
	mov es, ax
	mov bx, 0
	mov dl, 0
	mov dh, 0
	mov ch, 0
	mov cl, 4 ; starts at sector 4
	mov al, 15

	mov ah, 2
	int 0x13
	
	mov bx, 0x2e00
	mov [bx], byte 0
	
	mov ax, 0x2e0
	mov es, ax
	mov bx, 0
	mov dl, 0
	mov dh, 1 ; other side of this track contains the next sectors
	mov dl, 0
	mov ch, 0
	mov cl, 1 ; start at sector 1
	mov al, 18 ; read 18 sectors from this track
	mov ah, 2
	int 0x13

	; set the GFX chip to character mode
	mov al, 0x03
	mov ah, 0x00
	int 0x10

	; switch to protected mode...
	cli 
	lgdt[gdtr]
	mov eax,cr0
	or al,0x01
	mov cr0,eax
	jmp codesel:startKern
bits 32
startKern: 
	mov ax,datasel
	mov ds,ax						; Initialise ds & es to data segment 
	mov es,ax  
	mov ss,ax
	mov ax,videosel					; Initialise gs to video memory 
	mov gs,ax  
	;mov word [gs:0],0x0F4C ;  L
	;mov word [gs:2],0x0F6F ;  O
	;mov word [gs:4],0x0F61 ;  A
	;mov word [gs:6],0x0F64 ;  D
	;mov word [gs:8],0x0F69 ;  I
	;mov word [gs:10],0x0F6E ;  N
	;mov word [gs:12],0x0F67 ;  G
	;mov word [gs:14],0x0F20 ;  
	;mov word [gs:16],0x0F4B ;  K
	;mov word [gs:18],0x0F65 ;  E
	;mov word [gs:20],0x0F72 ; R
	;mov word [gs:22],0x0F6E ; N
	;mov word [gs:24],0x0F65 ; E
	;mov word [gs:26],0x0F6C ; L
	;mov word [gs:28],0x0F2E ; .
	;mov word [gs:30],0x0F2E ; .
	;mov word [gs:32],0x0F2E ; .
	jmp 0x1000
spin : jmp spin						; Loop 

bits 16 
gdtr : 
	dw gdt_end-gdt-1				; GDT table length
	dd gdt							; GDT table start
gdt
nullsel equ $-gdt			  		; nullsel = 0h
gdt0
	dd 0
	dd 0
codesel equ $-gdt
code_gdt
	dw 0x0ffff						; Limit 4Gb  bits 0-15 of segment descriptor 
	dw 0x0000						; Base 0h bits 16-31 of segment descriptor (sd) 
	db 0x00							; Base addr of seg 16-23 of 32bit addr,32-39 of sd  
	db 0x09a						; P,DPL(2),S,TYPE(3),A->Present bit 1,Descriptor  
			 						; privilege level 0-3,Segment descriptor 1 ie code			  
	db 0x0cf		  				; Upper 4 bits G,D,0,AVL ->1 segment len is page 
									; granular, 1 default operation size is 32bit seg		
									; Lower nibble bits 16-19 of segment limit 
	db 0x00							; Base addr of seg 24-31 of 32bit addr,56-63 of sd 
datasel equ $-gdt					; ie 10h, beginning of next 8 bytes for data sd 
data_gdt							; Data descriptor 4Gb flat seg at 0000:0000h 
	dw 0x0ffff						; Limit 4Gb 
	dw 0x0000						; Base 0000:0000h 
	db 0x00							; Descriptor format same as above 
	db 0x092 
	db 0x0cf 
	db 0x00 
videosel equ $-gdt					; ie 18h,next gdt entry 
	dw 3999							; Limit 80*25*2-1 
	dw 0x8000						; Base 0xb8000 
	db 0x0b 
	db 0x92							; present,ring 0,data,expand-up,writable 
	db 0x00							; byte granularity 16 bit 
	db 0x00
localTable equ $-gdt				; ie 20h,next gdt entry
	dw 0xf							; limit to 2 descriptors
	dw 0x8000						; base address
	db 0x0
	db 0x82							; TODO: figure out the right type
	db 0x1f
	db 0x0
tssEntry equ $-gdt					;ie 28h,next gdt entry
	dw 103							; full TSS
	dw 0x9000						; just store the TSS at 0x9000
	db 0x0
	db 0xE9							; magic TSS type
	db 0x0
	db 0x0
gdt_end

