#include "initrd.h"
#include "vfs.h"
#include "string.h"
#include "vga.h"


static uint32_t octal_to_int(const char* s) {
    uint32_t result = 0;
    while (*s) {
        result = result * 8 + (*s - '0');
        s++;
    }
    return result;
}

struct tar_header{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char type;
    char linkname[100];
    char padding[255];
}__attribute__((packed));

static fs_node nodes[64];
static tar_header* headers[64];
static uint32_t file_count = 0;

static uint32_t tarfs_read(fs_node* node, uint32_t offset, uint32_t size, char* buf) {
    tar_header* hdr = headers[node->inode];
    char* content = (char*)hdr + 512;
    
    if (offset > node->length) return 0;
    if (offset + size > node->length) size = node->length - offset;
    
    for (uint32_t i = 0; i < size; i++) {
        buf[i] = content[offset + i];
    }
    return size;
}


fs_node* initrd_init(uint32_t mod_start){
    tar_header* hdr = (tar_header*) mod_start;
    while(hdr->name[0] != '\0'){
        uint32_t file_size = octal_to_int(hdr->size);
        for(int i = 0; i<100; i++){
            nodes[file_count].name[i] = hdr->name[i];
        }
        nodes[file_count].flags = VFS_FILE;
        nodes[file_count].length = file_size;
        nodes[file_count].inode = file_count;
        nodes[file_count].read = tarfs_read;
        headers[file_count] = hdr;
        file_count++;
        uint32_t blocks = (file_size + 511) / 512;
        hdr = (tar_header*)((char*)hdr + 512 + blocks * 512);
    }
    return nodes;
}
fs_node* initrd_findfile(const char* file){
    for(uint32_t i = 0; i<file_count; i++){
        if(strcmp(nodes[i].name, file)){
            return &nodes[i];
        }
    }
    return 0;
}
void initrd_listfiles(){
    for(uint32_t i = 0; i<file_count; i++){
        vga_print(nodes[i].name);
        vga_print(" ");
    }
    vga_print("\n");
}