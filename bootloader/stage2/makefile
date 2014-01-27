MODULES		:=	drivers stdlib
SRCDIR		:=	src $(addprefix src/,$(MODULES))
BINDIR		:=	bin
BINMODS		:=	$(addprefix bin/,$(MODULES))
TARGET		:=	$(BINDIR)/stage2.bin
SRCS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.c')
ASMSRCS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.asm')
HEADERS		:=	$(shell find $(SRCDIR) -maxdepth 1 -name '*.h')
OBJECTS 	:=	$(subst src,$(BINDIR),$(SRCS:%.c=%.c.o)) $(subst src,$(BINDIR),$(ASMSRCS:%.asm=%.asm.o))

CFLAGS		:=	-m32 -Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs --no-builtin -ffreestanding -fleading-underscore  -std=c99 -g
LDFLAGS		:=	-T stage2.ld -O2 -nostdlib -g --oformat binary
ASMFLAGS	:=	-g
INCLUDES	:=	$(addprefix -I,$(SRCDIR))

AS 			:=	nasm
CC			:=	i586-elf-gcc
LD			:=	i586-elf-ld

vpath $(SRCDIR)

all: $(OBJECTS)
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