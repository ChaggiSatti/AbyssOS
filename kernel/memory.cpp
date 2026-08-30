#include "memory.h"
#include "vga.h"

uint32_t heap_start = 0x200000;
uint32_t current_heap_pointer;

void memory_init(){
    current_heap_pointer = heap_start;
}
void* malloc(size_t size){
    uint32_t allocated_address = current_heap_pointer;
    current_heap_pointer+=size;
    if(current_heap_pointer >= 0x2000000){
        vga_print("The Abyss is filled with your trash, mortal\n");
        while(1){
            asm volatile("cli; hlt");
        }
    }
    return (void*)allocated_address;
}