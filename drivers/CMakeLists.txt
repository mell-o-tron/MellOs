file(GLOB_RECURSE DRIVERS_C_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(FILTER DRIVERS_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(drivers STATIC ${DRIVERS_C_SRCS})
#target_link_libraries(drivers PUBLIC mellos_libc)
target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(drivers cpu_api)