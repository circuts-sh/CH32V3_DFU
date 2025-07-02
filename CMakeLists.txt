set(PROJECT_NAME "CH32V3_DFU")
cmake_minimum_required(VERSION 3.15)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR RISCV)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_FORCED 1)
set(CMAKE_CXX_COMPILER_FORCED 1)
set(CMAKE_ASM_COMPILER_FORCED 1)
set(CMAKE_LINKER_FORCED 1)
set(CMAKE_OBJCOPY_FORCED 1)
set(CMAKE_OBJDUMP_FORCED 1)
set(SIZE_FORCED 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)


set(COMPILER_PREFIX riscv-wch-elf)
set(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_LINKER ${COMPILER_PREFIX}-ld)
set(CMAKE_AR ${COMPILER_PREFIX}-ar)
set(CMAKE_OBJCOPY ${COMPILER_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${COMPILER_PREFIX}-objdump)
set(SIZE ${COMPILER_PREFIX}-size)

# Prevent CMake from testing the toolchain
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

enable_language(C CXX ASM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

project(CH32V3_DFU)
add_executable(${PROJECT_NAME}.elf "")

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -march=rv32imacxw
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Os
    -Wall
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -nostartfiles
    -Xlinker
    --gc-sections
    -Wl,-print-memory-usage
    --specs=nano.specs
    --specs=nosys.specs
)

# Set a default value for PORT_SPEED
set(PORT_SPEED "HS" CACHE STRING "USB Port Speed (HS or FS)")


# Add subdirectories for DFU and drivers
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ch32v30x_drivers)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dfu)



include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hid
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/cdc
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/msc
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/dfu
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hub
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/video
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/common
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/demo
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/core
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/ch32
)

# USB source
set(USB_CORE_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/core/usbd_core.c
)

if(${PORT_SPEED} STREQUAL "HS")
    list(APPEND USB_CORE_SOURCES 
        ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/ch32/ch32hs/usb_dc_usbhs.c
    )
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE -DCONFIG_USB_HS)
elseif(${PORT_SPEED} STREQUAL "FS")
    list(APPEND USB_CORE_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/ch32/ch32fs/usb_dc_usbfs.c
    )
endif()

set(USB_CLASS_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/dfu/usbd_dfu.c
)

target_sources(${PROJECT_NAME}.elf PRIVATE
    ${USB_CORE_SOURCES}
    ${USB_CLASS_SOURCES}
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_LIST_DIR}/Link.ld")

# Add post-build commands to generate BIN and HEX files and display memory usage
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex
    # Use size with -A flag to show all sections including RAM usage more verbosely
    COMMAND ${SIZE} -A  $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMAND ${COMPILER_PREFIX}-nm -S --size-sort --radix=d $<TARGET_FILE:${PROJECT_NAME}.elf> | tail -n 20
    COMMENT "Generating BIN and HEX files, displaying memory usage, and listing largest symbols"
)
