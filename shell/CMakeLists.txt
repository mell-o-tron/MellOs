add_library(shell_objs STATIC shell.c shell_functions.c functions/file_sys.c
        functions/displayinfo.c functions/vell.c functions/text.c functions/text_editor.c functions/exec.c functions/meminfo.c
        functions/misc.c functions/sperkaster/main.c functions/juggleballs/main.c functions/frogues/frogues.c
        functions/disk.c functions/mount.c functions/unix_files.c functions/roll.c functions/kill.c)
target_include_directories(shell_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(shell_objs vesa_objects text_editor_objs)