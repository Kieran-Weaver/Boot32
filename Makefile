CC=i686-elf-gcc
CXX=i686-elf-g++
CFLAGS= -Og -g
CPPFLAGS= -MT $@ -MMD -MP -MF build/$*.d -I stage3/include
CXXFLAGS= -Og -g -std=c++11 -fno-exceptions -fno-rtti
ASM=nasm
MUSL_SRCS=musl/src/string/i386/memcpy.s musl/src/string/i386/memmove.s \
musl/src/string/i386/memset.s
STAGE2_SRCS=$(shell find stage2/ -path "*.c")
STAGE3_SRCS=$(shell find stage3/ -path "*.c") $(shell find stage3/ -path "*.cpp") $(shell find stage3/ -path "*.cc") $(MUSL_SRCS)
STAGE2_OBJS=$(patsubst %.c, build/%.o, $(STAGE2_SRCS))
STAGE2_DEPS=$(patsubst %.c, build/%.d, $(STAGE2_SRCS))
STAGE3_OBJS=$(patsubst %.cc, build/%.o, $(patsubst %.s, build/%.o, $(patsubst %.cpp, build/%.o, $(patsubst %.c, build/%.o, $(STAGE3_SRCS)))))
STAGE3_DEPS=$(patsubst %.cc, build/%.d, $(patsubst %.s, build/%.d, $(patsubst %.cpp, build/%.d, $(patsubst %.c, build/%.d, $(STAGE3_SRCS)))))
MB=128
STAGE3_SECTORS=$(shell echo "$(MB) * 2048 - 51" | bc)
all: hda.img

%.bin: %.asm
	$(ASM) $^ -f bin -o $@

build/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) $^ -c -o $@

build/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) $^ -c -o $@

build/%.o: %.cc
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -mgeneral-regs-only $< -c -o $@ -ffreestanding

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -c -o $@ -ffreestanding

build/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -c -o $@ -ffreestanding

boot.bin: boot.asm
	nasm $^ -D MBR_LBA=$(STAGE3_SECTORS) -f bin -o $@

stage2.bin: stage2/stage2.ld build/stage2/crt0.o $(STAGE2_OBJS)
	cp build/stage2/crt0.o stage2/crt0.o
	$(CC) -Os -T stage2/stage2.ld $(STAGE2_OBJS) -o stage2.bin -nostdlib -ffreestanding
	rm -f stage2/crt0.o

root/kernel.elf: stage3/stage3.ld build/stage3/crt/crt0.o $(STAGE3_OBJS)
	mkdir -p root
	cp build/stage3/crt/crt0.o stage3/crt0.o
	$(CC) -Og -g -T stage3/stage3.ld $(STAGE3_OBJS) -o root/kernel.elf -nostdlib -ffreestanding
	rm -f stage3/crt0.o

hda.img: root/kernel.elf stage2.bin boot.bin
	./dir2fat32/dir2fat32.sh -f hda.img $(MB) root/
	dd if=boot.bin of=hda.img bs=512 conv=notrunc
	dd if=stage2.bin of=hda.img bs=512 seek=1 conv=notrunc

run: hda.img
	qemu-system-i386 -cpu pentium2 -accel kvm -hda hda.img

dbg: hda.img
	qemu-system-i386 -cpu pentium2 -s -S -hda hda.img > /dev/null 2>&1 &
	gdb root/kernel.elf

clean:
	rm -fr boot.bin stage2.bin build/ root/ hda.img

-include $(STAGE2_DEPS)
-include $(STAGE3_DEPS)
