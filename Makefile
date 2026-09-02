CC = i686-elf-g++
ASM = nasm
LD = i686-elf-ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib

all: abyss.iso

boot/boot.o: boot/boot.asm
	$(ASM) -f elf32 boot/boot.asm -o boot/boot.o

boot/gdt_flush.o: boot/gdt_flush.asm
	$(ASM) -f elf32 boot/gdt_flush.asm -o boot/gdt_flush.o

boot/idt_flush.o: boot/idt_flush.asm
	$(ASM) -f elf32 boot/idt_flush.asm -o boot/idt_flush.o

boot/interrupts.o: boot/interrupts.asm
	$(ASM) -f elf32 boot/interrupts.asm -o boot/interrupts.o

boot/paging.o: boot/paging.asm
	$(ASM) -f elf32 boot/paging.asm -o boot/paging.o

boot/usermode.o: boot/usermode.asm
	$(ASM) -f elf32 boot/usermode.asm -o boot/usermode.o

boot/task.o: boot/task.asm
	$(ASM) -f elf32 boot/task.asm -o boot/task.o

kernel/kernel.o: kernel/kernel.cpp
	$(CC) $(CFLAGS) -c kernel/kernel.cpp -o kernel/kernel.o

kernel/gdt.o: kernel/gdt.cpp
	$(CC) $(CFLAGS) -c kernel/gdt.cpp -o kernel/gdt.o

kernel/idt.o: kernel/idt.cpp
	$(CC) $(CFLAGS) -c kernel/idt.cpp -o kernel/idt.o

kernel/vga.o: kernel/vga.cpp
	$(CC) $(CFLAGS) -c kernel/vga.cpp -o kernel/vga.o

kernel/isr.o: kernel/isr.cpp
	$(CC) $(CFLAGS) -c kernel/isr.cpp -o kernel/isr.o

kernel/pic.o: kernel/pic.cpp
	$(CC) $(CFLAGS) -c kernel/pic.cpp -o kernel/pic.o

kernel/paging.o: kernel/paging.cpp	
	$(CC) $(CFLAGS) -c kernel/paging.cpp -o kernel/paging.o

kernel/string.o: kernel/string.cpp
	$(CC) $(CFLAGS) -c kernel/string.cpp -o kernel/string.o

kernel/tss.o: kernel/tss.cpp
	$(CC) $(CFLAGS) -c kernel/tss.cpp -o kernel/tss.o

kernel/memory.o: kernel/memory.cpp
	$(CC) $(CFLAGS) -c kernel/memory.cpp -o kernel/memory.o

kernel/timer.o: kernel/timer.cpp
	$(CC) $(CFLAGS) -c kernel/timer.cpp -o kernel/timer.o 

kernel/task.o: kernel/task.cpp
	$(CC) $(CFLAGS) -c kernel/task.cpp -o kernel/task.o

kernel/vfs.o: kernel/vfs.cpp
	$(CC) $(CFLAGS) -c kernel/vfs.cpp -o kernel/vfs.o 

kernel/initrd.o: kernel/initrd.cpp
	$(CC) $(CFLAGS) -c kernel/initrd.cpp -o kernel/initrd.o 

kernel/vesa.o: kernel/vesa.cpp
	$(CC) $(CFLAGS) -c kernel/vesa.cpp -o kernel/vesa.o

kernel/mouse.o: kernel/mouse.cpp
	$(CC) $(CFLAGS) -c kernel/mouse.cpp -o kernel/mouse.o 

kernel/ui.o: kernel/ui.cpp
	$(CC) $(CFLAGS) -c kernel/ui.cpp -o kernel/ui.o

kernel/ata.o: kernel/ata.cpp
	$(CC) $(CFLAGS) -c kernel/ata.cpp -o kernel/ata.o

abyss.bin: boot/boot.o kernel/kernel.o boot/gdt_flush.o boot/idt_flush.o boot/interrupts.o boot/paging.o kernel/gdt.o kernel/idt.o kernel/vga.o kernel/isr.o kernel/pic.o kernel/paging.o kernel/string.o boot/usermode.o kernel/tss.o kernel/memory.o kernel/timer.o kernel/task.o boot/task.o kernel/vfs.o kernel/initrd.o kernel/vesa.o kernel/mouse.o kernel/ui.o kernel/ata.o 
	$(CC) $(LDFLAGS) -o abyss.bin boot/boot.o kernel/kernel.o boot/gdt_flush.o boot/idt_flush.o boot/interrupts.o boot/paging.o kernel/gdt.o kernel/idt.o kernel/vga.o kernel/isr.o kernel/pic.o kernel/paging.o kernel/string.o boot/usermode.o kernel/tss.o kernel/memory.o kernel/timer.o kernel/task.o boot/task.o kernel/vfs.o kernel/initrd.o kernel/vesa.o kernel/mouse.o kernel/ui.o kernel/ata.o

abyss.iso: abyss.bin
	cp abyss.bin iso/boot/abyss.bin
	cd initrd && tar --format=ustar -cvf ../iso/boot/initrd.tar *
	grub-mkrescue -o abyss.iso iso

run: abyss.iso
# 	qemu-system-i386 -cdrom abyss.iso
	qemu-system-i386 -hda disk.img -cdrom abyss.iso -vga std

clean:
	rm -f boot/boot.o kernel/kernel.o abyss.bin abyss.iso iso/boot/abyss.bin boot/gdt_flush.o boot/idt_flush.o boot/interrupts.o boot/paging.o kernel/gdt.o kernel/idt.o kernel/vga.o kernel/isr.o kernel/pic.o kernel/paging.o kernel/string.o boot/usermode.o kernel/tss.o kernel/memory.o kernel/timer.o kernel/task.o boot/task.o kernel/vfs.o kernel/initrd.o kernel/vesa.o kernel/mouse.o kernel/ui.o kernel/ata.o
