#include "paging.h"

struct page_directory_t page_directory __attribute__((aligned(4096)));
struct page_table_t first_page_table __attribute__((aligned(4096)));

extern "C" void load_page_directory(uint32_t*);
extern "C" void enable_paging();

// void init_paging(){
//     for(int i = 0; i<1024; i++){
//         page_directory.tablesPhysical[i] = 0x02;
//     }
//     for(int i = 0; i<1024; i++){
//         first_page_table.page[i] = (i * 4096) | 7;
//     }
//     page_directory.tablesPhysical[0] = ((uint32_t)&first_page_table) | 7;
//     load_page_directory(page_directory.tablesPhysical);
//     enable_paging();
// }
void init_paging() {

    for (int i = 0; i < 1024; i++) {
        page_directory.tablesPhysical[i] = (i << 22) | 0x83;
    }
    load_page_directory(page_directory.tablesPhysical);
    enable_paging();
}
