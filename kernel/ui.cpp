#include "ui.h"
#include "vesa.h"
#include "mouse.h"
#include "fat32.h"

bool needs_redraw = true; // Always draw the very first frame

Window windows[4] = {
    {100, 100, 400, 300, "Abyss OS Settings", true, false, 0, 0},
    {550, 250, 400, 200, "System Monitor", true, false, 0, 0},
    {200, 150, 350, 250, "File Explorer", true, false, 0, 0},
    {300, 200, 400, 300, "Notepad", false, false, 0, 0}
};
int num_windows = 4;


// int num_windows = 2;

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
bool prev_mouse_left_pressed = false; 

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
            if(!w->visible) continue;
            if (w->is_dragged) {
                int new_x = mouse_x - w->drag_offset_x;
                int new_y = mouse_y - w->drag_offset_y;

                if (new_x < 0) new_x = 0;
                if (new_y < 0) new_y = 0;
                if (new_x + w->width > 1024) new_x = 1024 - w->width;
                if (new_y + w->height > 768) new_y = 768 - w->height;

                w->x = new_x;
                w->y = new_y;

                needs_redraw = true;
                return; // Exit early so we don't grab other windows!
            }
        }
    }

    // 3. If we aren't dragging anything yet, check for a NEW click!
    if(mouse_left_pressed and !prev_mouse_left_pressed){
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
}



void ui_render() {
    ui_update(); // Process mouse clicks first!
    
    if (!needs_redraw) return;
    
    // 1. Draw Background
    draw_gradient_rect(0, 0, 1024, 768, 0x100010, 0x004554);
    
    // 2. Draw Windows from our array
    for (int i = 0; i < num_windows; i++) {
        Window* w = &windows[i];
        
        // Skip hidden windows!
        if (!w->visible) continue;
        
        draw_window(w->x, w->y, w->width, w->height, w->title);
        
        // --- CUSTOM APP RENDERING ---
        
        // Shared buffer for our apps!
        static char notepad_text[2048];
        
        // Is this the File Explorer window?
        if (w->title[0] == 'F' && w->title[1] == 'i') { 
            draw_rect(w->x + 2, w->y + 24, w->width - 4, w->height - 26, 0xFFFFFF);
            
            static FAT32_DirectoryEntry files[16];
            static int file_count = -1; 
            if (file_count == -1) {
                file_count = fat32_get_root_entries(files, 16);
            }
            
            int file_y = w->y + 30;
            for (int f = 0; f < file_count; f++) {
                draw_rect(w->x + 10, file_y, 16, 16, 0x0055AA);
                
                char filename[13];
                int idx = 0;
                for(int j=0; j<8; j++) if(files[f].name[j] != ' ') filename[idx++] = files[f].name[j];
                filename[idx++] = '.';
                for(int j=0; j<3; j++) if(files[f].ext[j] != ' ') filename[idx++] = files[f].ext[j];
                filename[idx] = '\0';
                
                draw_string(filename, w->x + 35, file_y + 4, 0x000000, 0xFFFFFF, 1);
                
                // CLICK DETECTION
                if (mouse_left_pressed && !prev_mouse_left_pressed) {
                    if (mouse_x >= w->x + 10 && mouse_x <= w->x + 300 &&
                        mouse_y >= file_y && mouse_y <= file_y + 24) {
                        
                        for(int clr=0; clr<2048; clr++) notepad_text[clr] = 0;
                        
                        char search_name[9]; char search_ext[4];
                        for(int j=0; j<8; j++) search_name[j] = files[f].name[j];
                        search_name[8] = '\0';
                        for(int j=0; j<3; j++) search_ext[j] = files[f].ext[j];
                        search_ext[3] = '\0';
                        
                        fat32_read_file(search_name, search_ext, (uint8_t*)notepad_text);
                        
                        for(int win=0; win<num_windows; win++) {
                            if (windows[win].title[0] == 'N') windows[win].visible = true;
                        }
                        
                        needs_redraw = true;
                    }
                }
                
                file_y += 24;
            }
        }
        
        // Is this the Notepad window?
        if (w->title[0] == 'N' && w->title[1] == 'o') {
            draw_rect(w->x + 2, w->y + 24, w->width - 4, w->height - 26, 0xFFFFFF);
            draw_string(notepad_text, w->x + 10, w->y + 30, 0x000000, 0xFFFFFF, 1);
        }
        
        // ----------------------------
    }
    
    // 3. Draw Mouse
    mouse_draw();
    
    // 4. Swap buffers
    vesa_swap_buffers();
    
    needs_redraw = false;
    prev_mouse_left_pressed = mouse_left_pressed;
}


