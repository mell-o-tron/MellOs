file(GLOB_RECURSE SHELL_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(shell_objs STATIC ${SHELL_SRCS})
target_include_directories(shell_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(shell_objs vesa_objects text_editor_objs)