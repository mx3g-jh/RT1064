# Add set(CONFIG_USE_middleware_mcu-boot_drv_serial_nor_eeprom true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

      target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
          ${CMAKE_CURRENT_LIST_DIR}/src/drivers/serial_nor_eeprom/serial_nor_eeprom.c
        )

  
      target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
          ${CMAKE_CURRENT_LIST_DIR}/src/drivers/serial_nor_eeprom
        )

  
