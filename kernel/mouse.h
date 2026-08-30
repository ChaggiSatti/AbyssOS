#pragma once
#include <stdint.h>

extern int mouse_x;
extern int mouse_y;
extern uint8_t mouse_byte[3];
extern bool mouse_left_pressed;

void mouse_init();
void mouse_draw();
