file(GLOB_RECURSE KERNEL_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER KERNEL_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

enable_language(ASM_NASM)

file(GLOB_RECURSE KERNEL_ASM_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.asm)

set(KERNEL_SRCS ${KERNEL_C_SRCS} ${KERNEL_ASM_SRCS})

add_library(kernel STATIC ${KERNEL_SRCS})

string(APPEND CMAKE_ASM_NASM_FLAGS " -I${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(kernel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(kernel PRIVATE ${CMAKE_SOURCE_DIR}/global/include)
target_link_libraries(kernel memory memory_management_unit drivers data_structures shell_objs)