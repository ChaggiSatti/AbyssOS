#pragma once
#include <stdint.h>

struct Window {
    int x, y;
    int width, height;
    const char* title;
    bool visible;
    
    // State for dragging
    bool is_dragged;
    int drag_offset_x;
    int drag_offset_y;
};

extern bool needs_redraw;
void ui_render();
