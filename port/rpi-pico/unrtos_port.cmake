# Requires: UNRTOS_TARGET, UNRTOS_BASE_DIR, UNRTOS_PORT_DIR

# Idempotency: only add the port lib once
if(NOT TARGET unrtos_port_rpi_pico)
    add_subdirectory(${UNRTOS_BASE_DIR}/port/rpi-pico)
endif()

if(NOT TARGET ${UNRTOS_TARGET})
    message(FATAL_ERROR "unRTOS: target '${UNRTOS_TARGET}' does not exist (port/rpi-pico)")
endif()

target_link_libraries(${UNRTOS_TARGET} PUBLIC unrtos_port_rpi_pico)
