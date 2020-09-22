all: boot.bin

boot.bin: boot.asm
	nasm $^ -f bin -o $@

stage2.bin: stage2.asm
	nasm stage2.asm -f bin -o stage2.bin

hda.img: stage2.bin boot.bin
	dd if=boot.bin of=hda.img bs=512
	dd if=stage2.bin of=hda.img bs=512 oflag=append conv=notrunc

run: hda.img
	qemu-system-i386 -hda hda.img

clean:
	rm -fr boot.bin stage2.bin hda.img
