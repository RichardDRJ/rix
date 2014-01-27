SRCDIR		:=	src
BINDIR		:=	bin
BINMODS		:=	$(addprefix bin/,$(MODULES))
TARGET		:=	$(BINDIR)/stage1.bin

AS 			:=	nasm

all: $(BINDIR)/loader.bin

bootstraptest: $(BINDIR)/loader.bin $(BINDIR)/bootstraptest.bin

$(BINDIR)/bootstraptest.bin: $(SRCDIR)/bootstraptest.asm
	@mkdir -p $(BINDIR)
	$(AS) $< -o $@

$(BINDIR)/loader.bin: $(SRCDIR)/loader.asm
	@mkdir -p $(BINDIR)
	$(AS) $< -o $@

clean:
	rm -f BFYTW.BIN
	rm -f loader.bin
	rm -f start.img
	rm -f start.vdi
