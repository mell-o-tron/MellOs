file(GLOB_RECURSE LIBC_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER LIBC_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(mellos_libc STATIC ${LIBC_C_SRCS})
target_include_directories(mellos_libc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(mellos_libc PRIVATE ${CMAKE_SOURCE_DIR}/global/include)
target_link_libraries(mellos_libc utils)