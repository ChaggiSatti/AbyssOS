# Abyss OS

**Abyss OS** is a custom 32-bit x86 operating system built entirely from scratch. It is designed to be a lightweight, monolithic kernel that bridges the gap between raw hardware and graphical user interfaces. 

## 🚀 Features (Up to Phase 19)

- **Boot & Architecture:** Boots via GRUB into 32-bit Protected Mode.
- **CPU Management:** Custom Global Descriptor Table (GDT) and Interrupt Descriptor Table (IDT).
- **Hardware Interrupts:** Programmable Interrupt Controller (PIC) remapping and custom ISRs.
- **Memory Management:** Full Physical and Virtual Memory management (Paging).
- **Graphics:** High-resolution VESA Graphics Mode (1024x768x32bpp) with a double-buffered layout.
- **Text Rendering:** Custom software-rendered VGA terminal utilizing an 8x8 bitmap font.
- **Input Drivers:** 
  - Raw PS/2 Keyboard driver.
  - Raw PS/2 Mouse driver featuring a state machine and non-destructive cursor rendering (saving and restoring the background).
- **File System:** Virtual File System (VFS) interface mounted over a custom Initial Ramdisk (Initrd).
- **Multitasking:** Pre-emptive multitasking foundations with a Task State Segment (TSS).

## 🛠️ Build and Run

To compile and run Abyss OS, you will need a standard Linux toolchain.

### Prerequisites
- `nasm` (for assembly compilation)
- `g++` (i686-elf cross compiler or standard 32-bit multilib)
- `qemu-system-i386` (for emulation)
- `xorriso` and `grub-pc-bin` (for building the bootable ISO)

### Compilation

Clone the repository and run the build script:

```bash
make clean
make run
```
This will compile the kernel, link the binaries, package the Initial Ramdisk, generate the `abyss.iso` file, and boot it automatically in QEMU.

## 📖 Documentation

For a deep dive into the architecture, design choices, and bug-hunting history of Abyss OS, refer to the compiled LaTeX theory document located at:
`docs/Abyss_OS_Theory.pdf`
