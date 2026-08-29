#pragma once
#include <stdint.h>

struct Task{
    uint32_t id;
    uint32_t esp, ebp;
    uint32_t eip;
    Task* next;
};

void init_tasking();
void create_task(Task* t, void (*main)());
void switch_task();