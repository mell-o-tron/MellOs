add_library(drivers STATIC
        ramdisk.c disk.c keyboard.c port_io.c uart.c mouse.c vga_text.c)
#target_link_libraries(drivers PUBLIC mellos_libc)
target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(drivers cpu_api)