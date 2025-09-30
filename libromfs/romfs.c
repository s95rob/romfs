#include "romfs.h"

typedef struct {
    uint32_t magic,
        num_file_entries;
} romfs_header;

typedef struct {
    uint32_t hashed_name,
        bin_size,
        bin_offset;
} romfs_file_entry;

static void* romfs_base_addr = 0;
static romfs_header* header = 0;
static romfs_file_entry* file_entries = 0;

int romfs_init(void* base_addr) {
    // Validate the header chunk
    header = (romfs_header*)base_addr;
    if (header->magic != ROMFS_MAGIC)
        return 0;

    file_entries = (romfs_file_entry*)((uint8_t*)base_addr + sizeof(romfs_header));

    romfs_base_addr = base_addr;

    return 1;
}

int romfs_find(romfs_file* file, const char* filename) {
    uint32_t hashed_name = romfs_hash(filename);

    for (uint32_t i = 0; i < header->num_file_entries; i++) {
        if (file_entries[i].hashed_name == hashed_name) {
            file->data = (void*)((uint8_t*)romfs_base_addr +
                sizeof(romfs_header) + 
                file_entries[i].bin_offset);
            file->size = file_entries[i].bin_size;
            
            return 1;
        }
    }

    file->data = 0;
    file->size = 0;
    return 0;
}