#include "timer.h"
#include "io.h"

uint32_t tick_count = 0;
void timer_init(uint32_t frequency){
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    uint8_t low = (uint32_t)(divisor & 0xFF);
    uint8_t high = (uint32_t)((divisor >> 8) & 0xFF);
    outb(0x40, low);
    outb(0x40, high);
}
uint32_t timer_get_ticks(){
    return tick_count;
}
extern "C" void timer_tick(){
    tick_count++;
}