CC=i686-elf-gcc
ASM=nasm
STAGE2_SRCS=$(shell find stage2/ -path "*.c")
STAGE3_SRCS=$(shell find stage3/ -path "*.c")
STAGE2_OBJS=$(patsubst %.c, %.o, $(STAGE2_SRCS))
STAGE3_OBJS=$(patsubst %.c, %.o, $(STAGE3_SRCS))
MB=128
STAGE3_SECTORS=$(shell echo "$(MB) * 2048 - 51" | bc)
all: hda.img

%.bin: %.asm
	$(ASM) $^ -f bin -o $@

%.o: %.S
	$(CC) $^ -c -o $@

%.o: %.c
	$(CC) $^ -c -o $@ -ffreestanding

boot.bin: boot.asm
	nasm $^ -D MBR_LBA=$(STAGE3_SECTORS) -f bin -o $@

stage2.bin: stage2/stage2.ld stage2/crt0.o $(STAGE2_OBJS)
	$(CC) -Os -T stage2/stage2.ld $(STAGE2_OBJS) -o stage2.bin -nostdlib -ffreestanding

stage3/kernel.bin: stage3/stage3.ld stage3/crt0.o $(STAGE3_OBJS)
	$(CC) -Os -T stage3/stage3.ld $(STAGE3_OBJS) -o stage3/kernel.bin -nostdlib -ffreestanding

hda.img: stage3/kernel.bin stage2.bin boot.bin
	./dir2fat32/dir2fat32.sh -f hda.img $(MB) stage3/
	dd if=boot.bin of=hda.img bs=512 conv=notrunc
	dd if=stage2.bin of=hda.img bs=512 seek=1 conv=notrunc

run: hda.img
	qemu-system-i386 -hda hda.img

clean:
	rm -fr boot.bin stage2.bin stage3/kernel.bin hda.img stage2/*.o stage3/*.o
