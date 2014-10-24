CFLAGS	:=	-m32 -Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs --no-builtin -ffreestanding -fleading-underscore -I core -I drivers -I modules
CC		:=	i686-elf-gcc
LD		:=	i686-elf-ld

all: main
	$(LD) createstack.o readfat.o string.o system.o stdin.o harddiskdriver.o interrupts.o irqs.o isrs.o main.o screenprint.o keyboarddriver.o walloc.o -o BFYTW.BIN -m elf_i386 --oformat binary -T main.ld
	sudo mount -o loop start.img startmount/ && sudo cp BFYTW.BIN ../machine/ENIGMA.BIN ../machine/*.pb ../machine/*.rot startmount/ && sudo umount startmount

boottest: screenprint
	$(CC) -c boottest.c -o boottest.o $(CFLAGS)
	nasm createstack.asm -o createstack.o -f elf32
	$(LD) boottest.o createstack.o screenprint.o -o BFYTW.BIN -m elf_i386 --oformat binary -T main.ld
	sudo mount -o loop start.img startmount/ && sudo cp BFYTW.BIN startmount/ && sudo umount startmount


main: screenprint interrupts system keyboard stdin walloc string harddiskdriver readfat
	$(CC) -c main.c -o main.o $(CFLAGS)
	nasm createstack.asm -o createstack.o -f elf32
	
interrupts: screenprint system
	$(CC) -c interrupts.c -o interrupts.o $(CFLAGS)
	nasm irqs.asm -o irqs.o -f elf32
	nasm isrs.asm -o isrs.o -f elf32

screenprint:
	$(CC) -c screenprint.c -o screenprint.o $(CFLAGS)

keyboard: stdin
	$(CC) -c keyboarddriver.c -o keyboarddriver.o $(CFLAGS)

system:
	$(CC) -c system.c -o system.o $(CFLAGS)

stdin: walloc
	$(CC) -c stdin.c -o stdin.o $(CFLAGS)
	
walloc:
	$(CC) -c walloc.c -o walloc.o $(CFLAGS)

string:
	$(CC) -c string.c -o string.o $(CFLAGS)

harddiskdriver:
	$(CC) -c harddiskdriver.c -o harddiskdriver.o $(CFLAGS)

readfat: string harddiskdriver screenprint
	$(CC) -c readfat.c -o readfat.o $(CFLAGS)

clean:
	rm -f BFYTW.BIN
