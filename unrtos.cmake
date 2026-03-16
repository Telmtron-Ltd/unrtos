function(unrtos_use_port TARGET PORT CONFIG_DIR)
    if(NOT TARGET ${TARGET})
        message(FATAL_ERROR "unRTOS: target '${TARGET}' does not exist")
    endif()

    if(NOT DEFINED CONFIG_DIR)
        message(FATAL_ERROR "unRTOS: config directory '${CONFIG_DIR}' does not exist")
    endif()

    # Optional: UNRTOS_BASE_DIR keyword
    set(oneValueArgs UNRTOS_BASE_DIR)
    cmake_parse_arguments(UNRTOS "" "${oneValueArgs}" "" ${ARGN})

    if(NOT UNRTOS_UNRTOS_BASE_DIR)
        set(UNRTOS_BASE_DIR "${CMAKE_SOURCE_DIR}/unrtos") #default location
    else()
        set(UNRTOS_BASE_DIR "${UNRTOS_UNRTOS_BASE_DIR}")
    endif()
    get_filename_component(UNRTOS_BASE_DIR "${UNRTOS_BASE_DIR}" ABSOLUTE)

    set(_port_dir "${UNRTOS_BASE_DIR}/port/${PORT}")
    if(NOT EXISTS "${_port_dir}/unrtos_port.cmake")
        message(FATAL_ERROR "unRTOS: port '${PORT}' not found at ${_port_dir}")
    endif()

    # Expose vars to the included script
    set(UNRTOS_CONFIG_DIR ${CONFIG_DIR})
    set(UNRTOS_BASE_DIR "${UNRTOS_BASE_DIR}" PARENT_SCOPE)  # optional
    set(UNRTOS_PORT_DIR "${_port_dir}")
    set(UNRTOS_TARGET "${TARGET}")

    include("${_port_dir}/unrtos_port.cmake")
endfunction()
