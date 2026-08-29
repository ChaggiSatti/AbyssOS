#include "pic.h"
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

void pic_remap(int offset1, int offset2) {
    unsigned char a1, a2;
    
    // Save current masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);
    
    // Start initialization sequence in cascade mode
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    
    // Set offsets (this is where we map it to 32 and 40)
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();
    
    // Tell Master PIC that there is a Slave PIC at IRQ2 (0000 0100)
    outb(PIC1_DATA, 4);
    io_wait();
    
    // Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC2_DATA, 2);
    io_wait();
    
    // 8086/88 (MCS-80/85) mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
    
    // Restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

// End of Interrupt (EOI) command
void pic_send_eoi(unsigned char irq) {
    if(irq >= 8)
        outb(PIC2_COMMAND, 0x20); // Send to Slave
    
    outb(PIC1_COMMAND, 0x20); // Send to Master
}
