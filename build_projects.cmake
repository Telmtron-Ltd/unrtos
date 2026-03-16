
list(APPEND UNRTOS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/unrtos.c)
list(APPEND UNRTOS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/unwire/unwire.c)
list(APPEND UNRTOS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/hal/rp2040/timer.c)
list(APPEND UNRTOS_SOURCES ${CMAKE_CURRENT_LIST_DIR}/hal/rp2040/smp.c)

# Find all main.c files
message("Searching in '${CMAKE_SOURCE_DIR}'")
file(GLOB MAIN_C_PATHS "${CMAKE_SOURCE_DIR}/*/unrtos_project.cmake")
message("Found these directories: '${MAIN_C_PATHS}'")

foreach(MAIN_C_FILE ${MAIN_C_PATHS})
    get_filename_component(UNRTOS_PROJECT_DIR ${MAIN_C_FILE} DIRECTORY)
    get_filename_component(UNRTOS_PROJECT_NAME ${UNRTOS_PROJECT_DIR} NAME)

    add_library(${UNRTOS_PROJECT_NAME})
    include(${UNRTOS_PROJECT_DIR}/unrtos_project.cmake)

    # Build for each board version
    foreach(BOARD_VERSION IN LISTS BOARD_VERSIONS)    
        message("Building for board version: ${BOARD_VERSION}")

        # Build executable for the current board version
        set(EXEC_NAME "unrtos_${UNRTOS_PROJECT_NAME}_hw${BOARD_VERSION}")
        
        message("Found unrtos_project.cmake for '${UNRTOS_PROJECT_NAME}' (board v${BOARD_VERSION})")
        add_executable(${EXEC_NAME} ${UNRTOS_SOURCES} "${UNRTOS_PROJECT_DIR}/src/main.c")

        set(BOARD_LIB_TARGET "${BOARD_LIB_BASE}_hw${BOARD_VERSION}")
        include(${BOARD_LIB_DIR}/board_lib.cmake)
        
        target_link_libraries(${EXEC_NAME} PUBLIC 
            ${BOARD_LIB_TARGET}
            ${UNRTOS_PROJECT_NAME}
        )

        pico_add_extra_outputs(${EXEC_NAME})

        set(ARTIFACT_EXTS elf uf2 hex bin dis elf.map)
        add_custom_command(TARGET ${EXEC_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/${EXEC_NAME}")
        foreach(EXT IN LISTS ARTIFACT_EXTS)
            set(ARTIFACT "${EXEC_NAME}.${EXT}")
            set(TARGET_PATH "${CMAKE_BINARY_DIR}/${EXEC_NAME}/")
            add_custom_command(TARGET ${EXEC_NAME} POST_BUILD
                COMMENT "Moving ${ARTIFACT} to ${TARGET_PATH}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/${ARTIFACT}" "${TARGET_PATH}"
                COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_BINARY_DIR}/${ARTIFACT}"
            )
        endforeach()
    endforeach()
endforeach()

