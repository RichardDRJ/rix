MODULES		:=	drivers stdlib modules
SRCDIR		:=	src $(addprefix src/,$(MODULES))
BINDIR		:=	bin
BINMODS		:=	$(addprefix bin/,$(MODULES))
TARGET		:=	$(BINDIR)/stage2.bin
SRCS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.c')
ASMSRCS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.asm')
HEADERS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.h')
OBJECTS 	:=	$(subst src,$(BINDIR),$(ASMSRCS:%.asm=%.asm.o)) $(subst src,$(BINDIR),$(SRCS:%.c=%.c.o))

CFLAGS		:=	-gsplit-dwarf -m32 -Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs --no-builtin -ffreestanding -fleading-underscore  -std=c99
LDFLAGS		:=	-T stage2.ld -O2 -nostdlib -Map stage2.map -g --oformat binary
ASMFLAGS	:=	-g -felf
INCLUDES	:=	$(addprefix -I,$(SRCDIR))

AS 			:=	nasm
CC			:=	i686-elf-gcc
LD			:=	i686-elf-ld

vpath $(SRCDIR)

all: $(OBJECTS) makefile
	@echo "linking $(TARGET)..."
	@$(LD) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "linking complete"

$(BINDIR)/%.c.o: src/%.c $(BINMODS)
	@echo "creating object $@..."
	@$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

$(BINDIR)/%.asm.o: src/%.asm $(BINMODS)
	@echo "creating object $@..."
	@$(AS) $(ASMFLAGS) $< -o $@

$(BINMODS):
	mkdir -p $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -rf $(BINDIR)