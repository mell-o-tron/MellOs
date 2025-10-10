file(GLOB_RECURSE DISK_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(disk_interface STATIC ${DISK_SRCS})
target_include_directories(disk_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(disk_interface memory drivers)