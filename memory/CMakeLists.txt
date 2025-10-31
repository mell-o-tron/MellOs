file(GLOB_RECURSE MEMORY_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER MEMORY_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

# Helpful: export compile commands (a compile_commands.json will be in build dir)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(memory STATIC ${MEMORY_C_SRCS})
target_include_directories(memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(memory data_structures vesa_objects cpu_api kernel)
