SRCDIR		:=	src
BINDIR		:=	bin
BINMODS		:=	$(addprefix bin/,$(MODULES))
TARGET		:=	$(BINDIR)/stage1.bin

AS 			:=	nasm

all: $(TARGET)

bootstraptest: $(BINDIR)/loader.bin $(BINDIR)/bootstraptest.bin

$(BINDIR)/bootstraptest.bin: $(SRCDIR)/bootstraptest.asm
	@mkdir -p $(BINDIR)
	$(AS) $< -o $@

$(BINDIR)/stage1.bin: $(SRCDIR)/loader.asm
	@mkdir -p $(BINDIR)
	$(AS) $< -o $@

clean:
	rm -f $(TARGET)
