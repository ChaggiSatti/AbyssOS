#include "task.h"
#include "memory.h"
#include "vga.h"

Task* current_task;
Task* ready_queue;
uint32_t next_pid = 1;

void init_tasking(){
    asm volatile ("cli");
    current_task = (Task*)malloc(sizeof(Task));
    current_task->id = next_pid;
    next_pid++;
    current_task->esp = 0;
    current_task->ebp = 0;
    current_task->eip = 0;
    current_task->next = current_task;
    ready_queue = current_task;
    asm volatile ("sti");
}  

void create_task(Task* new_task, void (*main)()){
    uint32_t st = (uint32_t)malloc(4096);
    uint32_t* stack = (uint32_t*)(st + 4096);
    *(--stack) = 0x202;
    *(--stack) = 0x08;
    *(--stack) = (uint32_t)main;
    for(int i = 0; i<8; i++){
        *(--stack) = 0;
    }
    new_task->esp = (uint32_t)stack;
    new_task->ebp = (uint32_t)stack;
    new_task->eip = (uint32_t)main;
    new_task->id = next_pid++;
    new_task->next = current_task->next;
    current_task->next = new_task;
}