# Kernel source files

# kernel C sources
file(GLOB_RECURSE KERNEL_C_SRCS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/*.c)
list(FILTER KERNEL_C_SRCS EXCLUDE REGEX ".*/CMakeFiles/.*")
list(FILTER KERNEL_C_SRCS EXCLUDE REGEX ".*/external_modules/.*")

# Include directories
set(INC_DIRS)
list(APPEND INC_DIRS ${CMAKE_SOURCE_DIR})
list(APPEND INC_DIRS ${CMAKE_SOURCE_DIR}/lib)
list(REMOVE_DUPLICATES INC_DIRS)

# Assembly sources
set(KERNEL_ASM_SRCS
    ${CMAKE_SOURCE_DIR}/cpu/gdt/gdt_loader.asm
    ${CMAKE_SOURCE_DIR}/kernel/kernel_entry.asm
    ${CMAKE_SOURCE_DIR}/processes/processes_asm.asm
)