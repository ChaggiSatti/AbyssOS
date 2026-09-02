#pragma once
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val){
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

static inline void insw(uint16_t port, void* addr, uint32_t count) {
    asm volatile("cld; rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void outsw(uint16_t port, const void* addr, uint32_t count) {
    asm volatile("cld; rep outsw" : "+S"(addr), "+c"(count) : "d"(port) : "memory");
}
