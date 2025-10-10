file(GLOB_RECURSE GUI_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(gui_objs STATIC ${GUI_SRCS})
target_include_directories(gui_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(gui_objs mellos_libc vesa_objects memory)