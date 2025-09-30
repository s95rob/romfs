#ifndef ROMFS_H
#define ROMFS_H

#include <stdint.h>

#ifdef ROMFS_NO_INLINE
    #define ROMFS_INLINE
#else
    #define ROMFS_INLINE static inline
#endif


#define ROMFS_MAGIC 0x44415441 // "DATA" (little-endian) 


typedef struct {
    volatile void* data;
    uint32_t size;
} romfs_file;


ROMFS_INLINE uint32_t romfs_hash(const char* str) {
    uint32_t hash = 5381,
        c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}


int romfs_init(volatile void* base_addr);

int romfs_find(romfs_file* file, const char* filename);


#endif // ROMFS_H