add_library(drivers STATIC
        block_devices/ramdisk.c
        block_devices/pata_pio/parallel_ata_pio.c
        block_devices/pata_pio/lba28.c
        block_devices/pata_pio/lba48.c
        block_devices/pata_pio/ata_drive_info.c
        keyboard.c
        port_io.c
        uart.c
        mouse.c
        vga_text.c
        filesystems/devfs/devfs.c
        filesystems/procfs/procfs.c
        filesystems/fat/12/fat12.c
        filesystems/fat/fat_util.c
        filesystems/fat/12/fat12_rw.c
        rtc.c
)
#target_link_libraries(drivers PUBLIC mellos_libc)
target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(drivers cpu_api kernel_libc)
