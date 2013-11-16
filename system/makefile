MODULES		:=	core drivers modules stdlib
SRCDIR		:=	src $(addprefix src/,$(MODULES))
BINDIR		:=	bin
BINMODS		:=	$(addprefix bin/,$(MODULES))
TARGET		:=	$(BINDIR)/BFYTW.BIN
SRCS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.c')
ASMSRCS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.asm')
HEADERS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.h')
OBJECTS 	:=	$(subst src,$(BINDIR),$(SRCS:%.c=%.c.o)) $(subst src,$(BINDIR),$(ASMSRCS:%.asm=%.asm.o))

CFLAGS		:=	-m32 -Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs --no-builtin -ffreestanding -fleading-underscore  -std=c99 -g
LDFLAGS		:=	-T main.ld -ffreestanding -O2 -nostdlib -lgcc -g
ASMFLAGS	:=	-felf -g
INCLUDES	:=	$(addprefix -I,$(SRCDIR))

AS 			:=	nasm
CC			:=	i586-elf-gcc
LD			:=	i586-elf-gcc

vpath $(SRCDIR)

all: $(OBJECTS)
	make -C ../bootloader && cp ../bootloader/start.img $(BINDIR)
	$(LD) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	sudo mount -o users,loop $(BINDIR)/start.img startmount/
	sudo cp $(TARGET) startmount/
	sleep 1
	sudo umount startmount

vdi: start.img
	rm -f start.vdi
	VBoxManage convertfromraw -format VDI start.img start.vdi

boottest: $(OBJECTS)
	make -C ../bootloader && cp ../bootloader/start.img $(BINDIR)
	sudo mount -o users,loop $(BINDIR)/start.img startmount/
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFS) -c boottest.c -o $(BINDIR)/boottest.c.o
	$(LD) $(BINDIR)/boottest.c.o $(BINDIR)/core/createstack.asm.o $(BINDIR)/core/system.c.o $(BINDIR)/modules/kprint.c.o -o $(TARGET) $(LDFLAGS)
	sudo cp $(TARGET) startmount/
	sudo umount startmount

$(BINDIR)/%.c.o: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

$(BINDIR)/%.asm.o: src/%.asm
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
