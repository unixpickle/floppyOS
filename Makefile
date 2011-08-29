INCLUDEFLAG=-Isrc

compile: bin/bsect bin/kernel

bin/bsect: src/bootloader/bsect.s
	nasm -f bin src/bootloader/bsect.s -o bin/bsect

bin/kernel: bin/kern bin/testprog bin/kern/pit.o bin/kern/interrupt.o bin/kern/idtload.o bin/kern/floppyasm.o bin/kern/floppyc.o bin/kern/dma.o bin/kern/simplefloppy.o bin/kern/keyboard.o bin/kern/tasks.o bin/kern/switch.o bin/kern/lock.o bin/kern/kpi.o bin/kern/kmain.o bin/kern/kstdio.o bin/kern/kstdlib.o bin/kern/picinit.o bin/kern/idtinit.o
	nasm -f elf src/kernel/kernentry.s -o bin/kernentry.o	
	ld bin/kernentry.o bin/kern/* -Ttext 0x1000 -e kentry --oformat binary -s -o bin/kernel

bin/kern:
	mkdir bin/kern
	
bin/kern/idtinit.o:
	gcc -c src/kernel/idt/idtinit.c -fno-stack-protector -o bin/kern/idtinit.o $(INCLUDEFLAG)
	
bin/kern/picinit.o:
	nasm -f elf src/kernel/pic/picinit.s -o bin/kern/picinit.o

bin/kern/kstdlib.o: bin/kern
	gcc -c src/kernel/kstdlib.c -fno-stack-protector -o bin/kern/kstdlib.o $(INCLUDEFLAG)

bin/kern/kstdio.o: bin/kern
	gcc -c src/kernel/kstdio.c -fno-stack-protector  -o bin/kern/kstdio.o $(INCLUDEFLAG)

bin/kern/kmain.o: bin/kern
	gcc -c src/kernel/kmain.c -fno-stack-protector -o bin/kern/kmain.o $(INCLUDEFLAG)

bin/kern/kpi.o: bin/kern
	nasm -f elf src/kernel/kpi.s -o bin/kern/kpi.o

bin/kern/lock.o: bin/kern
	gcc -c src/kernel/tasks/lock.c -o bin/kern/lock.o -fno-stack-protector $(INCLUDEFLAG)

bin/kern/switch.o: bin/kern
	nasm -f elf src/kernel/tasks/switch.s -o bin/kern/switch.o

bin/kern/keyboard.o: bin/kern
	gcc -c src/kernel/keyboard.c -o bin/kern/keyboard.o -fno-stack-protector $(INCLUDEFLAG)

bin/kern/tasks.o: bin/kern
	gcc -c src/kernel/tasks/tasks.c -o bin/kern/tasks.o -fno-stack-protector $(INCLUDEFLAG)

bin/kern/simplefloppy.o: bin/kern
	gcc -c src/kernel/drives/simplefloppy.c -o bin/kern/simplefloppy.o -fno-stack-protector $(INCLUDEFLAG)

bin/kern/dma.o: bin/kern
	nasm -f elf src/kernel/drives/dma.s -o bin/kern/dma.o

bin/kern/idtload.o: bin/kern
	nasm -f elf src/kernel/idt/idtload.s -o bin/kern/idtload.o

bin/kern/interrupt.o: bin/kern
	nasm -f elf src/kernel/idt/interrupt.s -o bin/kern/interrupt.o

bin/kern/pit.o: bin/kern
	nasm -f elf src/kernel/idt/pit.s -o bin/kern/pit.o

bin/testprog:
	nasm -f bin src/testprog.s -o bin/testprog

bin/kern/floppyasm.o: bin/kern
	nasm -f elf src/kernel/drives/floppy.s -o bin/kern/floppyasm.o

bin/kern/floppyc.o: bin/kern
	gcc -c src/kernel/drives/floppy.c -o bin/kern/floppyc.o -fno-stack-protector $(INCLUDEFLAG)

image: bin/makeimg
	cp template.img output.img
	bin/makeimg output.img bin/bsect bin/kernel bin/testprog

bin/makeimg: src/makeimg.c
	gcc src/makeimg.c -o bin/makeimg

clean:
	rm -r bin/*

