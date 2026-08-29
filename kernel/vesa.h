#pragma once
#include <stdint.h>
uint8_t color_r(uint32_t c);
uint8_t color_g(uint32_t c);
uint8_t color_b(uint32_t c);
uint32_t get_pixel(uint32_t x, uint32_t y);

void draw_gradient_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h,uint32_t color1, uint32_t color2);
void vesa_init(uint32_t fb_addr, uint32_t width, uint32_t height, uint32_t pitch);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_circle(int xc, int yc, int r, uint32_t color);
void fill_circle(int xc, int yc, int r, uint32_t color);
void draw_char(char c, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color, int scale);
void draw_string(const char* str, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color, int scale);


