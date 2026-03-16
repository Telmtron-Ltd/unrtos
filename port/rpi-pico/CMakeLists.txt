# port/rpi-pico/CMakeLists.txt

cmake_minimum_required(VERSION 3.13)

# Bring in unRTOS core definitions
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/unrtos_core.cmake)

# Pico SDK-required boilerplate
# (Assumes the integrator already called pico_sdk_init())
add_library(unrtos_port_rpi_pico
    ${UNRTOS_CORE_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/timer.c
    ${CMAKE_CURRENT_LIST_DIR}/smp.c
    ${CMAKE_CURRENT_LIST_DIR}/sync.c
)

target_include_directories(unrtos_port_rpi_pico PUBLIC
    ${UNRTOS_INCLUDE_DIRS}   
)

target_link_libraries(unrtos_port_rpi_pico PUBLIC
    pico_stdlib
)
