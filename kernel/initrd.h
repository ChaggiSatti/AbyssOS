#pragma once
#include <stdint.h>
#include "vfs.h"

fs_node* initrd_init(uint32_t mod_start);
fs_node* initrd_findfile(const char* name);
void initrd_listfiles();