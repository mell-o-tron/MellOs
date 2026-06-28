# run targets

# Create the ISO using grub-mkrescue
set(ISO_NAME "mellos.iso")
set(ISO_PATH "${CMAKE_BINARY_DIR}/${ISO_NAME}")
set(KERNEL_DEST "${CMAKE_BINARY_DIR}/iso/boot/kernel.elf")
set(GRUB_DIR "${CMAKE_BINARY_DIR}/iso/boot/grub")
set(TEST_IMG_SOURCE "${CMAKE_SOURCE_DIR}/test_disk.img")
set(TEST_IMG_BINARY "${CMAKE_BINARY_DIR}/test_disk.img")

add_custom_target(prepare_grub_dir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GRUB_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/grub.cfg ${GRUB_DIR}/grub.cfg
)

# Copy test_disk.img to build dir. Doing this to avoid situations where
# it is accidentally committed. This behaviour will change in the future
add_custom_command(
        OUTPUT ${TEST_IMG_BINARY}
        COMMAND ${CMAKE_COMMAND} -E copy ${TEST_IMG_SOURCE} ${TEST_IMG_BINARY}
        COMMENT "Copying test_disk.img to build directory"
)

if (GRUB_MKRESCUE)

    add_custom_command(
            OUTPUT ${ISO_PATH}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_BIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel.elf> ${KERNEL_DEST}
            COMMAND ${GRUB_MKRESCUE} -o ${ISO_PATH} ${CMAKE_BINARY_DIR}/iso
            DEPENDS kernel.elf prepare_grub_dir
            VERBATIM
            COMMENT "Creating bootable ISO with grub-mkrescue"
    )
    add_custom_target(iso ALL DEPENDS ${ISO_PATH})
    add_custom_target(hda ALL DEPENDS ${TEST_IMG_BINARY})

    if (QEMU_SYS)

        add_custom_target(run
                COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -no-reboot -serial file:${CMAKE_BINARY_DIR}/serial.log -enable-kvm -cpu ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw ${ADDITIONAL_QEMU_FLAGS} -boot order=dc
                DEPENDS iso hda
                USES_TERMINAL
                COMMENT "Running MellOs in QEMU")
        
        add_custom_target(debug
                COMMAND ${CMAKE_COMMAND} --build . --target debug_symbols
                COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -s -vga std -enable-kvm -cpu ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw ${ADDITIONAL_QEMU_FLAGS} -boot order=dc
                DEPENDS iso hda
                USES_TERMINAL
                COMMENT "Run QEMU with gdb stub (-s) and std VGA")
        
        add_custom_target(novga_telnet
                COMMAND ${CMAKE_COMMAND} --build . --target debug_symbols
                COMMAND qemu-system-i386 -cdrom ${ISO_PATH} -m 128M -s -enable-kvm -cpu ${TARGET_CPU} -drive file=${TEST_IMG_BINARY},format=raw ${ADDITIONAL_QEMU_FLAGS} -boot order=dc -serial telnet:127.0.0.1:4444,server -nographic
                DEPENDS iso hda
                USES_TERMINAL
                COMMENT "Run QEMU with gdb stub (-s), no VGA, and bind UART I/O to telnet")

        endif()
        
endif()

if (CMAKE_OBJCOPY)

    # Debug symbol file equivalent to Makefile's kernel.sym
    add_custom_target(debug_symbols
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:kernel.elf> ${OUT_BIN_DIR}/kernel.sym
            DEPENDS kernel.elf
            COMMENT "Extracting debug symbols to kernel.sym")

endif()
        
