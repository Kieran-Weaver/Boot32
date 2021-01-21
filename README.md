# Boot32
A bootloader for 32-bit x86, targeted at i686 to Pentium 4 processors,
designed to be a 32-bit analogue to [BOOTBOOT](https://gitlab.com/bztsrc/bootboot).
The bootloader has 2 stages, and the "stage3" directory contains a test kernel.
Much of the code for this is based off of documentation on the [OSDev wiki](https://wiki.osdev.org/Main_Page).

### Goal
To turn i686-era laptops and mobile devices into primarily network-attached displays,
and also provide a Unix-like programming environment.

### Building / Testing
Building a hard drive image, hda.img: 
```
make
```

Running hda.img in QEMU (also builds hda.img):
```
make run
```

### Stage 1:
- Disables interrupts
- Relocates from 7C00h-7E00h to 600h-800h
- Gets an E820 memory map
- Reads stage 2 from disk
- Enables 32-bit Protected mode
- Stage 1 is also a valid MBR

### Stage 1 Memory Layout:
```
0     - 600h:   Reserved for BIOS
600h  - 800h:   Stage 1 code
800h  - 6C00h:  Stage 2 code
6C00h - 7C00h:  Stage 1 stack
7C00h - 9F00h:  E820 Entries (24 bytes each)
9F00h - 10000h: TSS (Unused)
```

### Stage 2:
- Changes stack to the top of low memory
- Locates the boot drive
- Identity maps the first 4M of the physical address space
- Maps the first 4M of free RAM above 4M to the last 4M of the virtual address space
- Loads the kernel from the first FAT32 partition on the boot drive to `p_vaddr` in the ELF header

### Stage 2 Memory Layout:
```
0     - 600h:   Reserved for BIOS
600h  - 800h:   Unused Stage 1 code
800h  - 6C00h:  Stage 2 code
6C00h - 7C00h:  Unused Stage 1 stack
7C00h - 24*N :  N E820 Entries
24*N  - SP   :  Stage 2 stack
```

### TODO:
- Add simple kernel
  - Use standard x86 hardware:
    - Enable FPU
    - Process APM, ACPI, SMBios
    - Handle Interrupts: IDT, PIC, APIC, IOAPIC
    - Handle Timers & Clocks: PIT, RTC, HPET
  - Make the kernel useful:
    - Create VM subsystem
    - Create STDIO driver
    - Create ATA driver
    - Create FS / VFS subsystem
