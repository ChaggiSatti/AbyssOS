#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "paging.h"
#include "tss.h"
#include "memory.h"
#include "timer.h"
#include "task.h"
#include "multiboot.h"
#include "vfs.h"
#include "initrd.h"
#include "vesa.h"
#include "mouse.h"
#include "ui.h"
#include "ata.h"
#include "fat32.h"

extern "C" void jump_usermode();
extern "C" void test_user_function(){
    const char* str = "Hello from User Mode via System Call!\n";
    asm volatile(
        "mov %0, %%eax;"
        "mov %1, %%ebx;"
        "int $0x80;"
        : : "r"(1), "r"(str) : "eax", "ebx"
    );
    
    while(1){}
}
const char* message = "The Abyss is staring at you, mortal!!\n";
extern "C" void perform_task_switch();
void task_a(){
    while(1){
        vga_print("A ");
    }
}
void task_b(){
    while(1){
        vga_print("B ");
    }
}
extern "C" void kernel_main(multiboot_info* mbd){
    gdt_init();
    uint32_t esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    tss_init(esp);
    idt_init();
    pic_remap(32, 40);
    init_paging();
    memory_init();
    timer_init(100);
    init_tasking();
    vesa_init((uint32_t)mbd->framebuffer_addr, mbd->framebuffer_width, mbd->framebuffer_height, mbd->framebuffer_pitch);
    mouse_init();
    ata_init();
    uint8_t disk_buf[512];
    for(int i = 0; i<512; i++) disk_buf[i] = 0;
    const char* disk_msg = "The Hard drive is ready mortal!";
    for(int i = 0; disk_msg[i] != '\0'; i++){
        disk_buf[i] = disk_msg[i];
    }
    ata_write_sector(1, disk_buf);
    vga_clear();
    draw_rect(0, 0, 1024, 768, 0x000000);
    fat32_init();
    fat32_list_root();
    vesa_swap_buffers();
    while(1);
    // draw_gradient_rect(0,0,1024,768, 0x100010, 0x004554);
    vga_print("Abyss v1.0 loading...\nLoading Virtual File System...\nFiles loaded...\n...\n...\nHello Mortal...");
    if (mbd->mods_count > 0) {
        multiboot_mod_list* mod = (multiboot_mod_list*)mbd->mods_addr;
        initrd_init(mod->mod_start);
        const char* name = "hell.txt";
        fs_node* file = initrd_findfile(name);
        if (file) {
            char buf[128] = {0};
            vfs_read(file, 0, file->length, buf);
            vga_print("VFS Read: ");
            vga_print(buf);
            vga_print("\n");
        }else{
            vga_print("Filename ");
            vga_print(name);
            vga_print(" not found!!\n");
        }
    }else{
        vga_print("ERROR : no initrd found!!");
    }
    vga_print(message);
    vga_print("You want help? HAHA!! Type 'help', mortal\n");
    vga_print("Abyss >> ");
    // Task* t1 = (Task*)malloc(sizeof(Task));
    // Task* t2 = (Task*)malloc(sizeof(Task));

    // create_task(t1, task_a);
    // create_task(t2, task_b);
    asm volatile ("sti");
    while(1){
        ui_render();
        asm volatile ("hlt");
    }
    // while(1){}
    // asm volatile ("int $0x8");
    // asm volatile ("sti");
    // jump_usermode();
    // while(1){}
}