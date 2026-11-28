file(GLOB_RECURSE PROCESSES_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER PROCESSES_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(processes STATIC ${PROCESSES_SRCS})
target_include_directories(processes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(processes kernel)