BINDIR		:=	bin
IMGTARGET	:=	$(BINDIR)/start.img
MNTPATH		:=	mnt
STAGE1PATH	:=	bootloader/stage1/bin/stage1.bin
STAGE2PATH	:=	bootloader/stage2/bin/stage2.bin
SYSTEMPATH	:=	system/bin/system.bin

.PHONY: install bootloader system clean all

all: bootloader system

install: bootloader system
	@echo "creating image..."
	@mkdir -p $(BINDIR)
	@dd if=/dev/zero of=$(IMGTARGET) bs=512 count=2048
	@echo "installing stage1..."
	@dd if=$(STAGE1PATH) of=$(IMGTARGET) bs=512 conv=notrunc
	@echo "mounting image..."
	@mkdir -p $(MNTPATH)
	@mount -o users,loop $(IMGTARGET) $(MNTPATH)
	@umount $(MNTPATH)
	@echo "installing stage2..."
	@cp $(STAGE2PATH) $(MNTPATH)
	@echo "installing system..."
	@cp $(SYSTEMPATH) $(MNTPATH)

bootloader:
	@make -C bootloader

system:
	@make -C system

clean:
	@make -C system clean
	@make -C bootloader clean