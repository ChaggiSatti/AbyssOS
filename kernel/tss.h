#pragma once
#include <stdint.h>

struct tss_entry_t {
    uint32_t prev_tss;
    uint32_t esp0; // The ring 0 stack pointer!
    uint32_t ss0;  // The ring 0 stack segment!
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

void tss_init(uint32_t kernel_stack);
