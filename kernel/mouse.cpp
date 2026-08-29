#include "mouse.h"
#include "io.h"
#include "vesa.h"
#include "pic.h"

int mouse_x = 512;
int mouse_y = 384;
uint8_t mouse_cycle = 0;
uint8_t mouse_byte[3];

// --- NEW ARROW DATA ---
// A classic 10x15 pointer arrow
const int CURSOR_W = 10;
const int CURSOR_H = 15;

const uint8_t cursor_bitmap[15][10] = {
    {1,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0},
    {1,2,2,2,2,2,2,1,0,0},
    {1,2,2,2,2,2,2,2,1,0},
    {1,2,2,2,2,2,2,2,2,1},
    {1,2,2,2,2,1,1,1,1,1},
    {1,2,2,1,2,1,0,0,0,0},
    {1,2,1,0,1,2,1,0,0,0},
    {1,1,0,0,1,2,1,0,0,0},
    {1,0,0,0,0,1,1,0,0,0}
};

// uint32_t cursor_bg[5][5]; // A 5x5 pixel buffer
uint32_t cursor_bg[15][10];  // Our new 10x15 buffer
// ----------------------

int old_mouse_x = 512;
int old_mouse_y = 384;

void mouse_wait(uint8_t type){
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) { if ((inb(0x64) & 1) == 1) return; }
    } else {
        while (timeout--) { if ((inb(0x64) & 2) == 0) return; }
    }
}

void mouse_write(uint8_t a_write) {
    mouse_wait(1);
    outb(0x64, 0xD4); 
    mouse_wait(1);
    outb(0x60, a_write);
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

void mouse_init() {
    uint8_t status;
    
    mouse_wait(1);
    outb(0x64, 0xA8);
    
    mouse_wait(1);
    outb(0x64, 0x20); 
    mouse_wait(0);
    status = (inb(0x60) | 2);
    
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);
    
    mouse_write(0xF6);
    mouse_read();
    
    mouse_write(0xF4);
    mouse_read();
    
    // register_interrupt_handler(44, mouse_handler);
}

extern "C" void isr44_handler() {
    uint8_t status = inb(0x64);
    if ((status & 0x20) == 0) {
        pic_send_eoi(12);
        return; 
    }
    
    mouse_byte[mouse_cycle] = inb(0x60); 

    if (mouse_cycle == 0 && (mouse_byte[0] & 0x08) == 0) {
        pic_send_eoi(12);
        return;
    }
    
    mouse_cycle++; 

    if (mouse_cycle == 3) {
        mouse_cycle = 0; 
        
        int d_x = (int8_t)mouse_byte[1]; 
        int d_y = (int8_t)mouse_byte[2]; 
        
        mouse_x += d_x;
        mouse_y -= d_y; 
        
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        
        // --- OLD 5x5 BOUNDS ---
        // if (mouse_x > 1023 - 5) mouse_x = 1023 - 5; 
        // if (mouse_y > 767 - 5) mouse_y = 767 - 5;   
        
        // --- NEW ARROW BOUNDS ---
        if (mouse_x > 1023 - CURSOR_W) mouse_x = 1023 - CURSOR_W; 
        if (mouse_y > 767 - CURSOR_H) mouse_y = 767 - CURSOR_H;   

        // --- OLD 5x5 DRAWING LOGIC ---
        // for (int i = 0; i < 5; i++) {
        //     for (int j = 0; j < 5; j++) {
        //         put_pixel(old_mouse_x + j, old_mouse_y + i, cursor_bg[i][j]);
        //     }
        // }
        // for (int i = 0; i < 5; i++) {
        //     for (int j = 0; j < 5; j++) {
        //         cursor_bg[i][j] = get_pixel(mouse_x + j, mouse_y + i);
        //     }
        // }
        // for (int i = 0; i < 5; i++) {
        //     for (int j = 0; j < 5; j++) {
        //         put_pixel(mouse_x + j, mouse_y + i, 0xFF00FF);
        //     }
        // }

        // --- NEW ARROW DRAWING LOGIC ---
        // 1. RESTORE
        for (int i = 0; i < CURSOR_H; i++) {
            for (int j = 0; j < CURSOR_W; j++) {
                if (cursor_bitmap[i][j] != 0) {
                    put_pixel(old_mouse_x + j, old_mouse_y + i, cursor_bg[i][j]);
                }
            }
        }

        // 2. SAVE
        for (int i = 0; i < CURSOR_H; i++) {
            for (int j = 0; j < CURSOR_W; j++) {
                if (cursor_bitmap[i][j] != 0) {
                    cursor_bg[i][j] = get_pixel(mouse_x + j, mouse_y + i);
                }
            }
        }

        // 3. DRAW
        for (int i = 0; i < CURSOR_H; i++) {
            for (int j = 0; j < CURSOR_W; j++) {
                if (cursor_bitmap[i][j] == 1) {
                    put_pixel(mouse_x + j, mouse_y + i, 0x000000); 
                } else if (cursor_bitmap[i][j] == 2) {
                    put_pixel(mouse_x + j, mouse_y + i, 0xFFFFFF); 
                }
            }
        }

        old_mouse_x = mouse_x;
        old_mouse_y = mouse_y;
    }
    
    pic_send_eoi(12); 
}

// void mouse_handler(registers_t* regs) {
//     uint8_t status = inb(0x64);
//     if ((status & 0x20) == 0) return; 

//     mouse_byte[mouse_cycle++] = inb(0x60);

//     if (mouse_cycle == 3) {
//         mouse_cycle = 0;

//         int d_x = (int)((char)mouse_byte[1]); 
//         int d_y = (int)((char)mouse_byte[2]); 
        
//         mouse_x += d_x;
//         mouse_y -= d_y;
        
//         if (mouse_x < 0) mouse_x = 0;
//         if (mouse_y < 0) mouse_y = 0;
//         if (mouse_x > 1023) mouse_x = 1023;
//         if (mouse_y > 767) mouse_y = 767;

//         put_pixel(mouse_x, mouse_y, 0xFF0000); 
//     }
// }
