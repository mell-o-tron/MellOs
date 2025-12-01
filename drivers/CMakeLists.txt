add_library(drivers STATIC
        ramdisk.c disk.c keyboard.c port_io.c uart.c mouse.c vga_text.c
        filesystems/devfs/devfs.c
        filesystems/procfs/procfs.c
        filesystems/fat/12/fat12.c
        filesystems/fat/fat_util.c
        filesystems/fat/12/fat12_rw.c
)
#target_link_libraries(drivers PUBLIC mellos_libc)
target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# for the fat12_rw.h
target_include_directories(drivers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/filesystems/fat/12)
target_link_libraries(drivers cpu_api)
