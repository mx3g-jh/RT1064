# Add set(CONFIG_USE_middleware_fatfs true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

      target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
          ${CMAKE_CURRENT_LIST_DIR}/source/diskio.c
          ${CMAKE_CURRENT_LIST_DIR}/source/ff.c
          ${CMAKE_CURRENT_LIST_DIR}/source/ffsystem.c
          ${CMAKE_CURRENT_LIST_DIR}/source/ffunicode.c
        )

  
      target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
          ${CMAKE_CURRENT_LIST_DIR}/source
        )

  
