file(GLOB_RECURSE DATA_STRUCTURES_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER DATA_STRUCTURES_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(data_structures STATIC
        allocator.c
        bitmap.c
        circular_buffer.c
        circular_list.c
        circular_queue.c
        hash_map.c
        linked_list.c
        list.c
)
target_include_directories(data_structures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(data_structures mellos_libc memory)