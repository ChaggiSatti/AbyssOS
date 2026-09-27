#include "vga.h"
#include "pic.h"
#include "io.h"
#include "keyboard.h"
#include "string.h"
#include "memory.h"
#include "timer.h"
#include "initrd.h"

struct regs_t {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;                 
    uint32_t eip, cs, eflags, useresp, ss;  
};

// const char* message = "The Abyss is staring at you, mortal!!\n";
extern char* message;
const char* exception_messages[32] = {
    "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds", "Invalid Opcode", "No Coprocessor",
    "Double Fault", "Coprocessor Segment Overrun", "Bad TSS", "Segment Not Present",
    "Stack Fault", "General Protection Fault", "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check", "Machine Check", "Reserved", 
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", 
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", 
    "Reserved", "Reserved"
};

extern "C" void fault_handler(const struct regs_t* r){
    if(r->int_no < 32){
        vga_clear();
        vga_print("Abyss is angered!! Your connection with Abyss is severed!!\n");
        vga_print(exception_messages[r->int_no]);
        while(1){
            asm volatile("cli");
            asm volatile("hlt");
        }
    }else if(r->int_no == 128){
        if(r->eax == 1){
            char* add = (char*)r->ebx;
            vga_print(add);
            
            // return;
        }
        return;
        // if(r->eax == 2){
        //     return;
        // }

    }
    
}
extern "C" void timer_tick(); 
char keyboard_buffer[256];
int buffer_ind = 0;
void execute_command(){
    keyboard_buffer[buffer_ind] = '\0';
    vga_print("\n");
    if(strcmp(keyboard_buffer, "clear")){
        vga_clear();
        vga_print(message);
    }
    else if(strcmp(keyboard_buffer, "hail")){
        vga_print("THE OLDEST DARKNESS\n");
    }
    else if(strcmp(keyboard_buffer, "help")){
        vga_print("These are the helping commands mortal :\n 1. clear\n 2. hail\n 3. alloc/malloc\n 4. uptime\n 5. cat <filename>\n 6. help\n 7. ls\n");
    }
    else if(strcmp(keyboard_buffer, "alloc") or strcmp(keyboard_buffer, "malloc")){
        char* dynamic_string = (char*)malloc(20);
        dynamic_string[0] = 'C';
        dynamic_string[1] = 'o';
        dynamic_string[2] = 'r';
        dynamic_string[3] = 'r';
        dynamic_string[4] = 'o';
        dynamic_string[5] = 'd';
        dynamic_string[6] = 'e';
        dynamic_string[7] = '\0';
        vga_print("The Abyss gave you some space and stated:\n");
        vga_print(dynamic_string);
        vga_print("\n");
    }
    else if(strcmp(keyboard_buffer, "uptime")){
        uint32_t time = timer_get_ticks();
        vga_print("The Abyss has been rampant for ");
        char ch[256];
        itoa(time, ch);
        vga_print(ch);
        vga_print(" ticks!!\n");
    }else if(keyboard_buffer[0] == 'c' and keyboard_buffer[1] == 'a' and keyboard_buffer[2] == 't' and keyboard_buffer[3] == ' '){
        char* filename = keyboard_buffer + 4;
        fs_node* file = initrd_findfile(filename);
        if(file){
            char buf[512] = {0};
            vfs_read(file, 0, file->length, buf);
            vga_print(buf);
            // vga_print("\n");
        }else{
            vga_print("No file named ");
            vga_print(filename);
            vga_print(" in inventory, mortal!!\n");
        }
    }else if(strcmp(keyboard_buffer, "ls")){
        initrd_listfiles();
    }
    else if(buffer_ind > 0){
        vga_print("Abyss knows no such thing as ");
        vga_print("' ");
        vga_print(keyboard_buffer);
        vga_print(" '");
        vga_print("\n");
    }
    buffer_ind = 0;
    vga_print("Abyss >> ");
}
extern "C" void isr3_handler(){
    vga_print("Interrupt 3 fired!!");
}
extern "C" void isr32_handler() {
    timer_tick();
    pic_send_eoi(0);
}

extern "C" void isr33_handler() {
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) { 
        char c = kbdus[scancode]; 
        
        if (c == '\n') {
            execute_command(); 
        } 
        else if (c == '\b') {
            if (buffer_ind> 0) {
                buffer_ind--;
                vga_print_char('\b');
            }
        } 
        else if (c != 0 && buffer_ind < 255) { 
            keyboard_buffer[buffer_ind] = c;
            buffer_ind++;
            vga_print_char(c);
        }
    }
    pic_send_eoi(1); 
}
