#pragma once
#pragma pack(1)
#include <stdint.h>

struct IDT{
    uint16_t base_low, sel;
    uint8_t zero, flags;
    uint16_t base_high;
};

struct IDTR{
    uint16_t limit;
    uint32_t base;
};

void idt_init();