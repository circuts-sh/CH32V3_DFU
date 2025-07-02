target_include_directories(${PROJECT_NAME}.elf PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Peripheral/inc
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Core
    ${CMAKE_CURRENT_LIST_DIR}/Debug
)

file(GLOB PERIPHERAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Peripheral/src/*.c)
file(GLOB DEBUG_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Debug/*.c)

target_sources(${PROJECT_NAME}.elf PRIVATE
    ${PERIPHERAL_SOURCES}
    ${DEBUG_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/ch32v30x_it.c
    ${CMAKE_CURRENT_LIST_DIR}/system_ch32v30x.c
    ${CMAKE_CURRENT_LIST_DIR}/Startup/startup_ch32v30x_D8C.S
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    -DCONFIG_USBDEV_ALIGN_CHECK_DISABLE
)


