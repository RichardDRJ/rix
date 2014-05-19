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

vdi: loader
	rm -f start.vdi
	VBoxManage convertfromraw -format VDI start.img start.vdi


clean:
	make -C stage1 clean
	make -C stage2 clean