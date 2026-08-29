#include "gdt.h"
#include "tss.h"
extern struct tss_entry_t tss;
GDT gdt[6];
GDTR gdtr;

void set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity){
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].access = access;
    gdt[index].granularity = ((limit >> 16) & 0x0F) | granularity;
}

extern "C" void gdt_flush(GDTR*);

void gdt_init(){
    set_entry(0,0,0,0,0);
    set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    uint32_t tss_base = (uint32_t)&tss;
    uint32_t tss_limit = sizeof(tss);
    set_entry(5, tss_base, tss_limit, 0xE9, 0x00);
    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint32_t)&gdt;
    gdt_flush(&gdtr);
}

