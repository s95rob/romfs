cmake_minimum_required(VERSION 3.16)

project(romfs)

add_library(romfs STATIC "libromfs/romfs.c")
target_include_directories(romfs PUBLIC "libromfs/include")

add_executable(romfs-pack "romfs-pack/main.c")
target_link_libraries(romfs-pack PRIVATE romfs)