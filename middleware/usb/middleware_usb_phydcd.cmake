# Add set(CONFIG_USE_middleware_usb_phydcd true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

      target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
          ${CMAKE_CURRENT_LIST_DIR}/dcd/usb_phydcd.c
        )

  
      target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
          ${CMAKE_CURRENT_LIST_DIR}/dcd
        )

  
