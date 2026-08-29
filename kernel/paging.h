#pragma once
#include <stdint.h>

struct page_table_t{
    uint32_t page[1024];
};
struct page_directory_t{
    uint32_t tablesPhysical[1024];
    struct page_table_t* tables[1024];
};
void init_paging();