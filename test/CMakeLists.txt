#add_executable(testing)
# Linker script and options (similar to: ld -T kernel/kernel.ld ...) Use gcc
# driver to link with script and without standard libs

# get_filename_component(LDSCRIPT "${CMAKE_SOURCE_DIR}/kernel/kernel.ld" ABSOLUTE)
# if(USE_CLANG)
#     target_link_options(testing PRIVATE
#         -T${LDSCRIPT}
#         -nostdlib
#     )
# else()
#     target_link_options(testing PRIVATE
#         -Wl,-T,${LDSCRIPT}
#         -Wl,--start-group
#         -no-pie
#         -nostdlib
#         -nodefaultlibs
#         -nostartfiles
#         -Wl,-m,elf_i386
#         -Wl,--end-group)
# endif()

# string(APPEND CMAKE_ASM_NASM_FLAGS " -I${CMAKE_CURRENT_SOURCE_DIR}")
# target_include_directories(testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(testing ${LIBS}) # libs is defined in the repository root CMakeLists.txt
