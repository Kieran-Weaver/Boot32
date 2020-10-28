CC=i686-elf-gcc
ASM=nasm
STAGE2_SRCS=$(shell find stage2/ -path "*.c")
STAGE2_OBJS=$(patsubst %.cpp, %.o, $(STAGE2_SRCS))
all: boot.bin stage2.bin

%.bin: %.asm
	$(ASM) $^ -f bin -o $@

%.o: %.S
	$(CC) $^ -c -o $@

%.o: %.c
	$(CC) $^ -c -o $@ -ffreestanding

boot.bin: boot.asm
	nasm $^ -f bin -o $@

stage2.bin: stage2/stage2.ld stage2/crt0.o $(STAGE2_OBJS)
	$(CC) -Os -T stage2/stage2.ld $(STAGE2_OBJS) -o stage2.bin -nostdlib -ffreestanding

hda.img: stage2.bin boot.bin
	dd if=boot.bin of=hda.img bs=512
	dd if=stage2.bin of=hda.img bs=512 oflag=append conv=notrunc

run: hda.img
	qemu-system-i386 -hda hda.img

clean:
	rm -fr boot.bin stage2.bin hda.img stage2/*.o