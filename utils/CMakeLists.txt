file(GLOB_RECURSE UTILS_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER UTILS_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(utils STATIC ${UTILS_C_SRCS})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(utils mellos_libc data_structures)