#include "romfs.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char** argv) {
    if (argc < 2)
        goto err_usage;

    const int num_file_entries = argc - 1;

    // Create file descriptor table
    size_t fdt_size = sizeof(FILE*) * num_file_entries;
    FILE** fdt = malloc(fdt_size);

    // Open and validate files
    for (int i = 0; i < argc - 1; i++) {
        fdt[i] = fopen(argv[i + 1], "rb");
        
        if (!fdt[i]) {
            printf("Failed to open file `%s' for reading\nExiting\n", argv[i + 1]);
            return 1;
        }
    }

    // Begin writing binary archive
    FILE* fbin = fopen("rom.bin", "wb");
    if (!fbin) {
        printf("Failed to open `rom.bin' for writing\nExiting\n");
        return 1;
    }

    // Write header section
    const uint32_t magic_number = ROMFS_MAGIC;
    fwrite(&magic_number, sizeof(uint32_t), 1, fbin);
    fwrite(&num_file_entries, sizeof(int), 1, fbin);

    // Write nametable chunk
    uint32_t bin_offset = 0;
    for (int i = 0; i < num_file_entries; i++) {
        // Write hashed file name
        uint32_t hashed_name = romfs_hash(argv[i + 1]);
        fwrite(&hashed_name, sizeof(uint32_t), 1, fbin);
        
        // Write binary byte length
        fseek(fdt[i], 0, SEEK_END);
        uint32_t bin_len = ftell(fdt[i]);
        fwrite(&bin_len, sizeof(uint32_t), 1, fbin);
        rewind(fdt[i]);

        // Write binary offset and append it
        fwrite(&bin_offset, sizeof(uint32_t), 1, fbin);
        bin_offset += bin_len;
    }

    // Write binary chunk and close the file from the table
    for (int i = 0; i < num_file_entries; i++) {
        fseek(fdt[i], 0, SEEK_END);
        size_t file_size = ftell(fdt[i]);
        
        uint8_t* file_data = malloc(file_size);
        rewind(fdt[i]);
        fread(file_data, sizeof(uint8_t), file_size, fdt[i]);
        fwrite(file_data, sizeof(uint8_t), file_size, fbin);

        free(file_data);
        fclose(fdt[i]);
    }

    fclose(fbin);

    return 0;

err_usage:
    printf("Usage: romfs-pack [file1] [file2] [file3] ...\n");

    return 1;
}