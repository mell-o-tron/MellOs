file(GLOB_RECURSE TESTING_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER TESTING_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(testing STATIC ${TESTING_C_SRCS})

string(APPEND CMAKE_ASM_NASM_FLAGS " -I${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(testing filesystem memory)