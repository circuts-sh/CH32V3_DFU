if(DEBUG)
ADD_DEFINITIONS(-DDEBUG=1)
endif()

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE -DUSBD_DFU_XFER_SIZE=256)
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE -DUSBD_DFU_APP_DEFAULT_ADD=0x8007000)
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE -DCONFIG_USBDEV_REQUEST_BUFFER_LEN=256)
set(FLASH_SIZE_KB 128)
set(FLASH_DFU_BOOTLDR_SIZE_KB 28)
math(EXPR FLASH_APP_SIZE_KB "${FLASH_SIZE_KB} - ${FLASH_DFU_BOOTLDR_SIZE_KB}")

configure_file(dfu_device_config.h.in ${CMAKE_BINARY_DIR}/dfu_device_config.h @ONLY)

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_BINARY_DIR}
)

file(GLOB DFU_APP_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.c)
target_sources(${PROJECT_NAME}.elf PRIVATE
    ${DFU_APP_SRC_FILES}
)
