# unrtos_core.cmake
# Provides UNRTOS_CORE_SOURCES and UNRTOS_INCLUDE_DIRS for use by ports.

set(UNRTOS_CORE_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../src/unrtos.c
    ${CMAKE_CURRENT_LIST_DIR}/../extensions/unwire/unwire.c
    
)

set(UNRTOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../extensions
    ${CMAKE_CURRENT_LIST_DIR}/../include
    ${CMAKE_CURRENT_LIST_DIR}/../src/hal
    ${CMAKE_CURRENT_LIST_DIR}/../src
    ${UNRTOS_CONFIG_DIR}
)
