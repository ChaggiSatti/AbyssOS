#pragma once
#pragma pack(1)
#include <stdint.h>
struct GDT{
    uint16_t limit_low, base_low;
    uint8_t base_middle, access, granularity, base_high;
};

struct GDTR{
    uint16_t limit;
    uint32_t base;
};

void gdt_init();