CC=i686-elf-gcc
ASM=nasm
all: boot.bin stage2.bin

%.bin: %.asm
	$(ASM) $^ -f bin -o $@

%.o: %.S
	$(CC) $^ -c -o $@

%.o: %.c
	$(CC) $^ -c -o $@ -ffreestanding

boot.bin: boot.asm
	nasm $^ -f bin -o $@

stage2.bin: stage2/stage2.ld stage2/stage2.o stage2/crt0.o
	$(CC) -T stage2/stage2.ld stage2/stage2.o -o stage2.bin -nostdlib -ffreestanding

hda.img: stage2.bin boot.bin
	dd if=boot.bin of=hda.img bs=512
	dd if=stage2.bin of=hda.img bs=512 oflag=append conv=notrunc

run: hda.img
	qemu-system-i386 -hda hda.img

clean:
	rm -fr boot.bin stage2.bin hda.img
