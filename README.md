# romfs

A tiny ROM-based filesystem library and packing tool for 32-bit embedded devices. Pack files into a flat binary archive and index by filename. 

## Building and Usage

`romfs` uses CMake for build system configuration. Clone this repository, add the repository as a CMake subdirectory and link against CMake target `romfs`. 

In your project's code, simply initialize `romfs` and use it to index your data:

```c
// Declared some place elsewhere...

#define MY_PROJECT_ROM_ADDR 0x08000000

void do_something_with_data(volatile void* data, uint32_t size);

// ...

#include "romfs.h"

if (!romfs_init((volatile void*)MY_PROJECT_ROM_ADDR)) {
    // romfs failed to initialize...
}

romfs_file my_file;

if (romfs_find(&my_file, "myfile1.txt")) {
    do_something_with_data(my_file.data, my_file.size);
} else {
    // romfs_find failed to find "myfile1.txt" ...
}

```

Additionally, there is an executable `romfs-pack`. This is a packing tool that will create a compiled ROM filesystem, `rom.bin`. It is designed to be integrated into build scripts, taking file names as command line arguments to be compiled.

Example:
```bash
romfs-pack myfile1.txt myfile2.png myfile3.mp3
```

## Specification

`romfs` uses a lightweight binary specification for indexing and storing files. The final produced binary file from the packing tool `romfs-pack` contains a header chunk, file entry chunk and binary chunk.  

### Header

Stores the magic number identifier (ASCII string "`DATA`") and number of file entries in the file entry chunk. 

| Name | Bytes |
|---|---|
| Magic number | 4 |
| Number of file entries | 4 |

### File entry chunk

Each file entry in the file entry chunk describes where and how large each file's associated binary data is. File names are stored hashed for size efficiency, and the binary offset points to the beginning of the file's binary data in relation to the binary chunk's beginning.  

| Name | Bytes |
|---|---|
| Hashed file name | 4 |
| Binary byte length | 4 |
| Binary offset | 4 | 

### Binary chunk

The final chunk of the ROM filesystem contains a flat binary blob of all packed files. 