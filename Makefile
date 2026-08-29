CC = gcc
LD = ld
NASM = nasm

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

C_SOURCES = \
	kernel/kernel.c \
	kernel/shell.c \
	kernel/memory.c \
	kernel/process.c \
	kernel/persistent_fs.c \
	kernel/ata.c \
	kernel/filesystem.c \
	kernel/gdt.c \
	kernel/interrupts.c \
	kernel/rtc.c \
	kernel/timer.c

C_OBJECTS = \
	kernel/kernel.o \
	kernel/shell.o \
	kernel/memory.o \
	kernel/process.o \
	kernel/persistent_fs.o \
	kernel/ata.o \
	kernel/filesystem.o \
	kernel/gdt_c.o \
	kernel/interrupts_c.o \
	kernel/rtc.o \
	kernel/timer.o

ASM_OBJECTS = \
	kernel/kernel_entry.o \
	kernel/gdt.o \
	kernel/interrupts_asm.o

all: SahalaOS.iso

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/kernel_entry.o: kernel/kernel_entry.asm
	$(NASM) -f elf32 $< -o $@

kernel/gdt.o: kernel/gdt.asm
	$(NASM) -f elf32 $< -o $@

kernel/interrupts_asm.o: kernel/interrupts.asm
	$(NASM) -f elf32 $< -o $@

kernel.elf: $(C_OBJECTS) $(ASM_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

SahalaOS.iso: kernel.elf
	mkdir -p iso/boot
	cp kernel.elf iso/boot/kernel.elf
	grub-file --is-x86-multiboot kernel.elf
	rm -f $@
	grub-mkrescue -o $@ iso

clean:
	rm -f kernel/*.o kernel.elf SahalaOS.iso

rebuild: clean all

.PHONY: all clean rebuild
