# CROSS COMPILER SETTING
set(CMAKE_SYSTEM_NAME Generic)
cmake_minimum_required(VERSION 3.10.0)

# THE VERSION NUMBER
SET (MCUXPRESSO_CMAKE_FORMAT_MAJOR_VERSION 2)
SET (MCUXPRESSO_CMAKE_FORMAT_MINOR_VERSION 0)

include(ide_overrides.cmake OPTIONAL)

if(CMAKE_SCRIPT_MODE_FILE)
  message("${MCUXPRESSO_CMAKE_FORMAT_MAJOR_VERSION}")
  return()
endif()


set(CMAKE_STATIC_LIBRARY_PREFIX)
set(CMAKE_STATIC_LIBRARY_SUFFIX)

set(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
set(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
set(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/build/)
set(LIBRARY_OUTPUT_PATH ${ProjDirPath}/build/)

set(PROJECT_NAME dev_cdc_vcom_freertos)
project(${PROJECT_NAME})

enable_language(ASM CXX C)

set(MCUX_BUILD_TYPES debug flexspi_nor_debug flexspi_nor_release release sdram_debug sdram_release)

set(MCUX_SDK_PROJECT_NAME ${PROJECT_NAME}.elf)

if (NOT DEFINED SdkRootDirPath)
    SET(SdkRootDirPath ${ProjDirPath})
endif()

include(${ProjDirPath}/flags.cmake)

include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME} 
"${ProjDirPath}/src/usb_device_descriptor.c"
"${ProjDirPath}/src/usb_device_descriptor.h"
"${ProjDirPath}/src/virtual_com.c"
"${ProjDirPath}/src/virtual_com.h"
"${ProjDirPath}/src/hardware_init.c"
"${ProjDirPath}/src/pin_mux.c"
"${ProjDirPath}/src/pin_mux.h"
"${ProjDirPath}/src/board.h"
"${ProjDirPath}/src/board.c"
"${ProjDirPath}/src/clock_config.h"
"${ProjDirPath}/src/clock_config.c"
"${ProjDirPath}/src/FreeRTOSConfig_Gen.h"
"${ProjDirPath}/src/fsl_os_abstraction_config.h"
"${ProjDirPath}/src/mcux_config.h"
"${ProjDirPath}/src/usb_device_config.h"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${ProjDirPath}/src/
)

set_source_files_properties("${ProjDirPath}/src/FreeRTOSConfig_Gen.h" PROPERTIES COMPONENT_CONFIG_FILE "middleware_freertos-kernel_config")
set_source_files_properties("${ProjDirPath}/src/fsl_os_abstraction_config.h" PROPERTIES COMPONENT_CONFIG_FILE "component_osa_template_config")
set_source_files_properties("${ProjDirPath}/src/usb_device_config.h" PROPERTIES COMPONENT_CONFIG_FILE "middleware_usb_device_ehci_config_header")

include(${SdkRootDirPath}/devices/MIMXRT1064/all_lib_device.cmake)

IF(NOT DEFINED TARGET_LINK_SYSTEM_LIBRARIES)  
    SET(TARGET_LINK_SYSTEM_LIBRARIES "-lm -lc -lgcc -lnosys")  
ENDIF()  

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES})

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary
        ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME}
        ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -Oihex
        ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME}
        ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex
)

set_target_properties(${MCUX_SDK_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES "output.map;${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin")

# wrap all libraries with -Wl,--start-group -Wl,--end-group to prevent link order issue
group_link_libraries()

