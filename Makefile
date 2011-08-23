INCLUDEFLAG=-Isrc

compile: bin/bsect bin/kernel

bin/bsect: src/bootloader/bsect.s
	nasm -f bin src/bootloader/bsect.s -o bin/bsect

bin/kernel: bin/testprog bin/pit.o bin/interrupt.o bin/idtload.o bin/floppyasm.o bin/floppyc.o bin/dma.o bin/simplefloppy.o bin/keyboard.o bin/tasks.o bin/switch.o
	nasm -f elf src/kernel/kpi.s -o bin/kpi.o
	nasm -f elf src/kernel/kernentry.s -o bin/kernentry.o
	gcc -c src/kernel/kmain.c -fno-stack-protector -o bin/kmain.o $(INCLUDEFLAG)
	gcc -c src/kernel/kstdio.c -o bin/kstdio.o $(INCLUDEFLAG)
	nasm -f elf src/kernel/pic/picinit.s -o bin/picinit.o
	gcc -c src/kernel/idt/idtinit.c -fno-stack-protector -o bin/idtinit.o $(INCLUDEFLAG)
	gcc -c src/kernel/kstdlib.c -fno-stack-protector -o bin/kstdlib.o $(INCLUDEFLAG)
	nasm -f elf src/kernel/tasks/launch.s -o bin/launch.o
	ld bin/kernentry.o bin/kmain.o bin/kstdio.o bin/launch.o bin/kstdlib.o bin/kpi.o bin/picinit.o bin/idtinit.o bin/idtload.o bin/interrupt.o bin/pit.o bin/floppyc.o bin/floppyasm.o bin/dma.o bin/simplefloppy.o bin/keyboard.o bin/tasks.o bin/switch.o -Ttext 0x1000 -e kentry --oformat binary -s -o bin/kernel
	nasm -f bin src/kernel/test.s -o bin/test

bin/switch.o:
	nasm -f elf src/kernel/tasks/switch.s -o bin/switch.o

bin/keyboard.o:
	gcc -c src/kernel/keyboard.c -o bin/keyboard.o -fno-stack-protector $(INCLUDEFLAG)

bin/tasks.o:
	gcc -c src/kernel/tasks/tasks.c -o bin/tasks.o -fno-stack-protector $(INCLUDEFLAG)

bin/simplefloppy.o:
	gcc -c src/kernel/drives/simplefloppy.c -o bin/simplefloppy.o -fno-stack-protector $(INCLUDEFLAG)

bin/dma.o:
	nasm -f elf src/kernel/drives/dma.s -o bin/dma.o

bin/idtload.o:
	nasm -f elf src/kernel/idt/idtload.s -o bin/idtload.o

bin/interrupt.o:
	nasm -f elf src/kernel/idt/interrupt.s -o bin/interrupt.o

bin/pit.o:
	nasm -f elf src/kernel/idt/pit.s -o bin/pit.o

bin/testprog:
	nasm -f bin src/testprog.s -o bin/testprog

bin/floppyasm.o:
	nasm -f elf src/kernel/drives/floppy.s -o bin/floppyasm.o

bin/floppyc.o:
	gcc -c src/kernel/drives/floppy.c -o bin/floppyc.o -fno-stack-protector $(INCLUDEFLAG)

image: bin/makeimg
	cp template.img output.img
	bin/makeimg output.img bin/bsect bin/kernel bin/testprog

bin/makeimg: src/makeimg.c
	gcc src/makeimg.c -o bin/makeimg

clean:
	rm bin/*

