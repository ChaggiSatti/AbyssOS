#include "mouse.h"
#include "io.h"
#include "vesa.h"
#include "pic.h"
#include "ui.h" // We will create this next!

int mouse_x = 512;
int mouse_y = 384;
uint8_t mouse_cycle = 0;
uint8_t mouse_byte[3];
bool mouse_left_pressed = false; 

const int CURSOR_W = 10;
const int CURSOR_H = 15;
const uint8_t cursor_bitmap[15][10] = {
    {2,0,0,0,0,0,0,0,0,0},
    {2,2,0,0,0,0,0,0,0,0},
    {2,1,2,0,0,0,0,0,0,0},
    {2,1,1,2,0,0,0,0,0,0},
    {2,1,1,1,2,0,0,0,0,0},
    {2,1,1,1,1,2,0,0,0,0},
    {2,1,1,1,1,1,2,0,0,0},
    {2,1,1,1,1,1,1,2,0,0},
    {2,1,1,1,1,1,1,1,2,0},
    {2,1,1,1,1,1,1,1,1,2},
    {2,1,1,1,1,2,2,2,2,2},
    {2,1,1,2,1,2,0,0,0,0},
    {2,1,2,0,2,1,2,0,0,0},
    {2,2,0,0,2,1,2,0,0,0},
    {2,0,0,0,0,2,2,0,0,0}
};

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
}

void mouse_draw() {
    for (int i = 0; i < CURSOR_H; i++) {
        for (int j = 0; j < CURSOR_W; j++) {
            if (cursor_bitmap[i][j] == 1) {
                put_pixel(mouse_x + j, mouse_y + i, 0x000000); 
            } else if (cursor_bitmap[i][j] == 2) {
                put_pixel(mouse_x + j, mouse_y + i, 0xFFFFFF); 
            }
        }
    }
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
        mouse_left_pressed = (mouse_byte[0] & 0x01);
        
        int d_x = (int8_t)mouse_byte[1]; 
        int d_y = (int8_t)mouse_byte[2]; 
        
        mouse_x += d_x;
        mouse_y -= d_y; 
        
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x > 1023 - CURSOR_W) mouse_x = 1023 - CURSOR_W; 
        if (mouse_y > 767 - CURSOR_H) mouse_y = 767 - CURSOR_H;   
        
        // Signal the UI to draw a new frame!
        needs_redraw = true;
    }
    pic_send_eoi(12); 
}
