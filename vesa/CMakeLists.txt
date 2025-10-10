include_directories(include)

file(GLOB_RECURSE VESA_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(vesa_objects STATIC ${VESA_SOURCES})
target_include_directories(vesa_objects PUBLIC include)
target_link_libraries(vesa_objects mellos_libc)