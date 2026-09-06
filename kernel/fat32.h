#pragma once
#include <stdint.h>

// Packed ensures the compiler doesn't add padding between variables!
// #pragma pack(push, 1)

// The FAT32 Volume Boot Record (VBR) located at Sector 0
struct FAT32_VBR {
    uint8_t  jump[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;      // Usually 512
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;      // Boot record sectors before the FAT
    uint8_t  fat_count;             // Number of FATs (usually 2)
    uint16_t dir_entries;           // Unused in FAT32 (0)
    uint16_t total_sectors_16;      // Unused in FAT32 (0)
    uint8_t  media_descriptor;
    uint16_t sectors_per_fat_16;    // Unused in FAT32 (0)
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;      // Total sectors in volume
    
    // FAT32 Extended Boot Record
    uint32_t sectors_per_fat_32;    // Size of the FAT in sectors
    uint16_t flags;
    uint16_t fat_version;
    uint32_t root_cluster;          // Usually cluster 2
    uint16_t fsinfo_cluster;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  nt_flags;
    uint8_t  signature;             // 0x28 or 0x29
    uint32_t serial_number;
    char     label[11];
    char     sys_identifier[8];     // "FAT32   "
    uint8_t  boot_code[420];
    uint16_t boot_signature;        // 0xAA55
}__attribute__((packed));

// A 32-byte FAT32 Directory Entry
struct FAT32_DirectoryEntry {
    char     name[8];               // Short filename (e.g., "HELLO   ")
    char     ext[3];                // Extension (e.g., "TXT")
    uint8_t  attributes;            // File, Directory, Hidden, Read-Only, etc.
    uint8_t  reserved;
    uint8_t  creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t first_cluster_high;    // High 16 bits of the data cluster
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t first_cluster_low;     // Low 16 bits of the data cluster
    uint32_t size;                  // File size in bytes
}__attribute__((packed));

// #pragma pack(pop)

void fat32_init();
void fat32_list_root();
