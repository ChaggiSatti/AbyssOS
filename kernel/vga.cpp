#include "vga.h"
#include "vesa.h"
// volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
// int row = 0, col = 0;
// int cur_pos = 0;
uint32_t TERM_START_X = 0;
uint32_t TERM_START_Y = 0;
uint32_t TERM_MAX_X = 1024;
uint32_t TERM_MAX_Y = 768;

uint32_t term_x = TERM_START_X;
uint32_t term_y = TERM_START_Y;

const int FONT_SCALE = 2;
const uint32_t TERM_FG = 0xFFFFFF;
const uint32_t TERM_BG = 0x000000;


void vga_clear(){
    // for(int i = 0; i<2000; i++){
    //     vga[i] = (0x0F << 8) | ' ';
    // }
    // row = 0;
    // col = 0;
    // cur_pos = 0;
    draw_rect(TERM_START_X, TERM_START_Y, TERM_MAX_X, TERM_MAX_Y, TERM_BG);
    term_x = TERM_START_X;
    term_y = TERM_START_Y;
}
// void vga_putchar(char ch){
//     if(col == 80){
//         col = 0;
//         row++;
//         vga_putchar(ch);
//         return;
//     }
//     if(ch == '\n'){
//         col = 0;
//         row++;
//     }else{
//         vga[row * 80 + col] = (0x0F << 8) | ch;
//         col++;
//     }
// }
void scroll(){
    // if(cur_pos >= 2000){
    //     for(int i = 0; i < 24 * 80; i++){
    //         vga[i] = vga[i + 80];
    //     }
    //     for (int i = 24 * 80; i < 25 * 80; i++) {
    //         vga[i] = (0x0F << 8) | ' '; 
    //     }
    //     cur_pos = 24 * 80;
    // }
    if(term_y >= TERM_MAX_Y) vga_clear();
}
void vga_print_char(const char c){
    if(term_x >= TERM_MAX_X){
        // vga_print_char('\n');
        term_x = TERM_START_X;
        term_y += 8 * FONT_SCALE;
    }
    if(c == '\b'){
        // if(cur_pos > 0){
        //     cur_pos--;
        //     vga[cur_pos] = (0x0F << 8) | ' ';
        // }
        if(term_x > TERM_START_X){
            term_x -= 8 * FONT_SCALE;
            draw_rect(term_x, term_y, 8 * FONT_SCALE, 8 * FONT_SCALE, TERM_BG);
        }
    } else if (c == '\n') {
        // cur_pos = cur_pos + 80 - (cur_pos % 80);
        term_x = TERM_START_X;
        term_y += 8 * FONT_SCALE;
    } else {
        // vga[cur_pos] = (0x0F << 8) | c;
        // cur_pos++;
        draw_char(c, term_x, term_y, TERM_FG, TERM_BG, FONT_SCALE);
        term_x+=8 * FONT_SCALE;
    }
    scroll();
}
void vga_print(const char* s){
    for(int i = 0; s[i] != '\0'; i++){
        vga_print_char(s[i]);
    }
}