#include "ui.h"
#include "vesa.h"
#include "mouse.h"

bool needs_redraw = true; // Always draw the very first frame

Window windows[2] = {
    {100, 100, 400, 300, "Abyss OS Settings", false, 0, 0},
    {550, 250, 400, 200, "System Monitor", false, 0, 0}
};
int num_windows = 2;

void draw_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* title) {
    // Window Body
    draw_rect(x, y, width, height, 0xCCCCCC); // Light gray
    
    // Window Borders (Creates a slight 90s 3D pop-out effect)
    draw_rect(x, y, width, 1, 0xFFFFFF); // Top
    draw_rect(x, y, 1, height, 0xFFFFFF); // Left
    draw_rect(x + width - 1, y, 1, height, 0x555555); // Right
    draw_rect(x, y + height - 1, width, 1, 0x555555); // Bottom
    
    // Title Bar
    draw_rect(x + 2, y + 2, width - 4, 20, 0x0000AA); // Dark blue title bar
    draw_string(title, x + 6, y + 8, 0xFFFFFF, 0x0000AA, 2); // White text
}

bool is_any_window_dragged = false;

void ui_update() {
    // 1. If the mouse is released, drop everything
    if (!mouse_left_pressed) {
        for (int i = 0; i < num_windows; i++) {
            windows[i].is_dragged = false;
        }
        is_any_window_dragged = false;
        return;
    }

    // 2. If we are currently dragging a window, ONLY update that specific window
    if (is_any_window_dragged) {
        for (int i = 0; i < num_windows; i++) {
            Window* w = &windows[i];
            if (w->is_dragged) {
                w->x = mouse_x - w->drag_offset_x;
                w->y = mouse_y - w->drag_offset_y;
                needs_redraw = true;
                return; // Exit early so we don't grab other windows!
            }
        }
    }

    // 3. If we aren't dragging anything yet, check for a NEW click!
    for (int i = num_windows - 1; i >= 0; i--) {
        Window* w = &windows[i];
        
        // Check collision with the ENTIRE WINDOW (so clicking the body brings it to front)
        if (mouse_x >= w->x && mouse_x <= w->x + w->width &&
            mouse_y >= w->y && mouse_y <= w->y + w->height) {
            
            // Bring to front! (If it's not already on top)
            if (i != num_windows - 1) {
                Window clicked_window = windows[i];
                
                // Shift all windows above it down by 1 position
                for (int j = i; j < num_windows - 1; j++) {
                    windows[j] = windows[j + 1];
                }
                
                // Put our clicked window at the very end of the array (the top)
                windows[num_windows - 1] = clicked_window;
                
                // Re-point 'w' to the new top window so dragging works
                w = &windows[num_windows - 1];
            }
            
            // Check if they specifically clicked the Title Bar to start dragging
            if (mouse_y <= w->y + 24) {
                w->is_dragged = true;
                w->drag_offset_x = mouse_x - w->x;
                w->drag_offset_y = mouse_y - w->y;
                is_any_window_dragged = true;
            }
            
            needs_redraw = true; // Force redraw to show it popped to the front!
            break; 
        }
    }
}



void ui_render() {
    ui_update(); // Process mouse clicks first!
    
    if (!needs_redraw) return;
    // 1. Draw Background
    draw_gradient_rect(0, 0, 1024, 768, 0x100010, 0x004554);
    
    // 2. Draw Windows from our array
    for (int i = 0; i < num_windows; i++) {
        draw_window(windows[i].x, windows[i].y, windows[i].width, windows[i].height, windows[i].title);
    }
    // 3. Draw Mouse
    mouse_draw();
    // 4. Swap buffers
    vesa_swap_buffers();
    
    needs_redraw = false;
}


