#pragma once
#include <stdint.h>
#define VFS_FILE 0x01
#define VFS_DIRECTORY 0x02

struct fs_node{
    char name[128];
    uint32_t flags; // like is the file a file or a directory
    uint32_t length;
    uint32_t inode;
    uint32_t (*read)(fs_node*, uint32_t offset, uint32_t size, char* buf);
};
uint32_t vfs_read(fs_node* node, uint32_t offset, uint32_t size, char* buf);