#include "loader.h"
#include "vga.h"
#include "ui.h"

#define USER_LOAD_ADDRESS 0x400000

extern char notepad_text[2048];
extern int num_windows;
extern Window windows[4];

// Forward declare the kernel print function
extern void vga_print(const char* str);


void sys_open_notepad(const char* text){
    int i = 0;
    while(text[i] != '\0' && i < 2047){
        notepad_text[i] = text[i];
        i++;
    }
    notepad_text[i] = '\0';

    for(int win = 0; win < num_windows; win++){
        if(windows[win].title[0] == 'N') windows[win].visible = 1;
    }
}
void load_and_run(uint8_t* binary, uint32_t size) {
    uint8_t* dest = (uint8_t*)USER_LOAD_ADDRESS;
    for (uint32_t i = 0; i < size; i++) {
        dest[i] = binary[i];
    }

    // Call the program passing vga_print as a function pointer argument
    typedef void (*program_t)(void (*print_fn)(const char*));
    program_t program = (program_t)USER_LOAD_ADDRESS;
    program(sys_open_notepad);
}
