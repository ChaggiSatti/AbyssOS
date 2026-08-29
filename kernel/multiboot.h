#pragma once
#include <stdint.h>

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    
    uint64_t framebuffer_addr;    // The physical RAM address where the screen starts
    uint32_t framebuffer_pitch;   // How many BYTES make up one horizontal row of pixels
    uint32_t framebuffer_width;   // Screen width in pixels (1024)
    uint32_t framebuffer_height;  // Screen height in pixels (768)
    uint8_t  framebuffer_bpp;     // Bits Per Pixel (32 = 4 bytes per pixel)
    uint8_t  framebuffer_type;    // 1 = RGB color mode
    uint8_t  color_info[6];       // Which bits are Red, Green, Blue
} __attribute__((packed));

struct multiboot_mod_list {
    uint32_t mod_start; 
    uint32_t mod_end; 
    uint32_t cmdline;
    uint32_t pad;
} __attribute__((packed));
