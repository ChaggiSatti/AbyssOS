#include "tss.h"

struct tss_entry_t tss;

void tss_init(uint32_t kernel_stack) {
    uint8_t* tss_bytes = (uint8_t*)&tss;
    for (uint32_t i = 0; i < sizeof(tss); i++) {
        tss_bytes[i] = 0;
    }
    tss.ss0 = 0x10;
    tss.esp0 = kernel_stack;
    tss.iomap_base = sizeof(tss);
    asm volatile("ltr %%ax" : : "a"(0x2B));
}
