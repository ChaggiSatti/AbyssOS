#include "fat32.h"
#include "ata.h"
#include "vga.h"

uint32_t fat_start_sector = 0;
uint32_t data_start_sector = 0;
uint32_t root_dir_cluster = 0;
uint8_t  sectors_per_cluster = 0;

// Converts a FAT32 Cluster Number into a physical ATA LBA Sector Number
uint32_t cluster_to_lba(uint32_t cluster) {
    return data_start_sector + ((cluster - 2) * sectors_per_cluster);
}

void fat32_init() {
    uint8_t boot_sector[512];
    ata_read_sector(0, boot_sector); // Read LBA 0
    
    FAT32_VBR* vbr = (FAT32_VBR*)boot_sector;
    
    // Check if it's actually FAT32
    if (vbr->boot_signature != 0xAA55) {
        vga_print("Error: Disk is not FAT32 formatted!\n");
        return;
    }
    
    sectors_per_cluster = vbr->sectors_per_cluster;
    root_dir_cluster = vbr->root_cluster;
    
    // Calculate memory regions
    fat_start_sector = vbr->reserved_sectors;
    uint32_t fat_size = vbr->sectors_per_fat_32;
    data_start_sector = fat_start_sector + (vbr->fat_count * fat_size);
    
    vga_print("FAT32 Init Success! Sys ID: ");
    for(int i=0; i<8; i++) vga_print_char(vbr->sys_identifier[i]);
    vga_print("\n");
}

void fat32_list_root() {
    uint8_t root_buffer[512];
    uint32_t root_lba = cluster_to_lba(root_dir_cluster);
    
    // Read the first sector of the root directory
    ata_read_sector(root_lba, root_buffer);
    
    FAT32_DirectoryEntry* entries = (FAT32_DirectoryEntry*)root_buffer;
    
    // A 512-byte sector holds exactly sixteen 32-byte directory entries (512 / 32 = 16)
    for (int i = 0; i < 16; i++) {
        FAT32_DirectoryEntry* entry = &entries[i];
        
        // 0x00 means the rest of the directory is empty
        if (entry->name[0] == 0x00) break;
        
        // 0xE5 means the file was deleted, skip it
        if ((unsigned char)entry->name[0] == 0xE5) continue;
        
        // Skip hidden files, system files, and long-filename entries (0x0F)
        if (entry->attributes == 0x0F || (entry->attributes & 0x02) || (entry->attributes & 0x04)) continue;
        
        // Print the short filename!
        vga_print("Found File: ");
        for(int j=0; j<8; j++) if(entry->name[j] != ' ') vga_print_char(entry->name[j]);
        vga_print_char('.');
        for(int j=0; j<3; j++) if(entry->ext[j] != ' ') vga_print_char(entry->ext[j]);
        vga_print("\n");
    }
}
