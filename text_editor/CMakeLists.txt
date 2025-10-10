file(GLOB_RECURSE TEXT_EDITOR_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(text_editor_objs STATIC ${TEXT_EDITOR_SRCS})
target_include_directories(text_editor_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(text_editor_objs mellos_libc vesa_objects)