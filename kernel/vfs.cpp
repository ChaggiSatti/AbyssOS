#include "vfs.h"

uint32_t vfs_read(fs_node* node, uint32_t offset, uint32_t size, char* buf){
    if(node->read != 0){
        return node->read(node, offset, size, buf);
    }
    return 0;
}