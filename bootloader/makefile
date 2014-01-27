.PHONY: all stage1 stage2 clean

all: stage1 stage2

stage1:
	make -C stage1

stage2:
	make -C stage2

bootstraptest: loader
	nasm bootstraptest.asm -o BFYTW.BIN
	mkdir -p startmount
	sudo mount -o loop start.img startmount
	sudo cp BFYTW.BIN startmount
	sleep 1
	sudo umount startmount

loader:
	nasm loader.asm -o loader.bin
	dd if=/dev/zero of=start.img bs=512 count=2048
	dd if=loader.bin of=start.img bs=512 conv=notrunc

vdi: loader
	rm -f start.vdi
	VBoxManage convertfromraw -format VDI start.img start.vdi


clean:
	rm -f BFYTW.BIN
	rm -f loader.bin
	rm -f start.img
	rm -f start.vdi