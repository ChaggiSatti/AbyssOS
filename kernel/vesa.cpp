#include "vesa.h"
#include "font8x8.h"
#include "memory.h"

// #include <stdlib.h>

uint32_t* framebuffer;
uint32_t* backbuffer;
static uint32_t  fb_width;
static uint32_t  fb_height;
static uint32_t  fb_pitch;

void vesa_init(uint32_t addr, uint32_t width, uint32_t height, uint32_t pitch) {
    framebuffer = (uint32_t*)addr;
    fb_width    = width;
    fb_height   = height;
    fb_pitch    = pitch;

    backbuffer = (uint32_t*)malloc(width * height * 4);
}
void vesa_swap_buffers() {
    uint32_t total_pixels = fb_width * fb_height;
    uint32_t* src = backbuffer;
    uint32_t* dst = framebuffer;
    
    // Hardware-accelerated memory copy
    asm volatile (
        "cld; rep movsl"
        : "+S" (src), "+D" (dst), "+c" (total_pixels)
        :
        : "memory"
    );
}


void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_width || y >= fb_height) return;
    uint32_t index = y * fb_width + x;
    backbuffer[index] = color;
    // uint32_t offset = y * (fb_pitch / 4) + x;
    // framebuffer[offset] = color;
}
uint8_t color_r(uint32_t c){
    return (c >> 16) & 0xFF;
}
uint8_t color_g(uint32_t c){
    return (c >> 8) & 0xFF;
}
uint8_t color_b(uint32_t c){
    return c & 0xFF;
}
uint32_t get_pixel(uint32_t x, uint32_t y) {
    if (x >= fb_width || y >= fb_height) return 0;
    uint32_t offset = y * (fb_pitch / 4) + x;
    return framebuffer[offset];
}

void draw_gradient_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color1, uint32_t color2){
    for(uint32_t i = 0; i < h; i++){
        uint8_t r1 = color_r(color1);
        uint8_t r2 = color_r(color2);

        uint8_t g1 = color_g(color1);
        uint8_t g2 = color_g(color2);

        uint8_t b1 = color_b(color1);
        uint8_t b2 = color_b(color2);

        uint8_t r = (int)r1 + (((int)r2 - (int)r1) * (int)i) / (int)h;
        uint8_t g = (int)g1 + (((int)g2 - (int)g1) * (int)i) / (int)h;
        uint8_t b = (int)b1 + (((int)b2 - (int)b1) * (int)i) / (int)h;

        uint32_t color = (r << 16) | (g << 8) | b;
        draw_rect(x, y + i, w, 1, color);
    }
}
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color){
    // Safety clipping
    if (x >= fb_width || y >= fb_height) return;
    if (x + width > fb_width) width = fb_width - x;
    if (y + height > fb_height) height = fb_height - y;
    
    for (uint32_t i = 0; i < height; i++) {
        // Calculate the row exactly once
        uint32_t* row_ptr = &backbuffer[(y + i) * fb_width + x];
        
        // Fast fill across the row
        for (uint32_t j = 0; j < width; j++) {
            row_ptr[j] = color;
        }
    }
}

//Bresenham's line algo...
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1); //abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = (y1 > y0) ? (y0 - y1) : (y1 - y0);//-abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        put_pixel(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) {
            break;
        }
        
        e2 = 2 * err;
        
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}
void draw_circle(int xc, int yc, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        put_pixel(xc + x, yc + y, color); // Octant 1
        put_pixel(xc + y, yc + x, color); // Octant 2
        put_pixel(xc - y, yc + x, color); // Octant 3
        put_pixel(xc - x, yc + y, color); // Octant 4
        put_pixel(xc - x, yc - y, color); // Octant 5
        put_pixel(xc - y, yc - x, color); // Octant 6
        put_pixel(xc + y, yc - x, color); // Octant 7
        put_pixel(xc + x, yc - y, color); // Octant 8

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}
void fill_circle(int xc, int yc, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        // Draw horizontal lines connecting the mirrored points
        draw_line(xc - x, yc + y, xc + x, yc + y, color);
        draw_line(xc - x, yc - y, xc + x, yc - y, color);
        draw_line(xc - y, yc + x, xc + y, yc + x, color);
        draw_line(xc - y, yc - x, xc + y, yc - x, color);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}
/*
void draw_char(char c, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color){
    for(uint8_t row = 0; row < 8; row++){
        for(uint8_t col = 0; col < 8; col++){
            uint8_t row_data = font8x8_basic[(uint8_t)c][row];
            if(row_data >> col){
                put_pixel(x + col, y + row, fg_color);
            }else{
                put_pixel(x + col, y + row, bg_color);
            }
        }
    }
}
void draw_string(const char* str, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color){
    uint32_t start_x = x;
    while (*str != '\0') {
        if (*str == '\n') {
            x = start_x;
            y += 8;
        } else {
            draw_char(*str, x, y, fg_color, bg_color);
            x += 8;   
        }
        str++;
    }
}
*/

void draw_char(char c, uint32_t x, uint32_t y, uint32_t fg_color, [[maybe_unused]]uint32_t bg_color, int scale) {
    uint8_t* glyph = font8x8_basic[(uint8_t)c];
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((glyph[row] >> col) & 1) {
                draw_rect(x + (col * scale), y + (row * scale), scale, scale, fg_color);
            } else {
                // draw_rect(x + (col * scale), y + (row * scale), scale, scale, bg_color);
            }
        }
    }
}

void draw_string(const char* str, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color, int scale){
    uint32_t start_x = x;
    while(*str != '\0'){
        if (*str == '\n') {
            x = start_x;
            y += (8 * scale); 
        } else {
            draw_char(*str, x, y, fg_color, bg_color, scale);
            x += (8 * scale);
        }
        str++;
    }
}


