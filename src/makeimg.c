/**
 * Writes the kernel and bootloader to
 * a template floppy image file.
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define bootbuf_len 512*64

int main (int argc, char * argv[]) {
	char * boot_buf = (char *)malloc(bootbuf_len);
	bzero(boot_buf, bootbuf_len);
	if (argc != 5) {
		fprintf(stderr, "Usage: %s <image.img> <bootloader> <kernel> <testprog>\r\n", argv[0]);
		exit(1);
	}

	FILE * bsect = fopen(argv[2], "r");
	FILE * kernel = fopen(argv[3], "r");
	FILE * testprog = fopen(argv[4], "r");
	FILE * fp = fopen(argv[1], "r+");

	// bootloader
	fread(boot_buf, 1, 512, bsect);
	fseek(fp, 64, SEEK_SET);
	fwrite(boot_buf, 1, 446, fp);

	// kernel (sector 4 and up)
	bzero(boot_buf, bootbuf_len);
	fseek(kernel, 0, SEEK_END);
	int length = ftell(kernel);
	printf("Kernel length: %d\n", length);
	fseek(kernel, 0, SEEK_SET);
	int i;
	for (i = 0; i < length; i++) {
		boot_buf[i] = fgetc(kernel);
	}
	fseek(fp, 512*3, SEEK_SET);
	fwrite(boot_buf, 1, length, fp);
	
	// testprog (sector 20)
	bzero(boot_buf, 512);
	fread(boot_buf, 1, 512, testprog);
	fseek(fp, 512*2, SEEK_SET);
	fwrite(boot_buf, 1, 512, fp);

	// close
	fclose(kernel);
	fclose(bsect);
	fclose(testprog);
	fclose(fp);
}
