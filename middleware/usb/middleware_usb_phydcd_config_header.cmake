# Add set(CONFIG_USE_middleware_usb_phydcd_config_header true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

            add_config_file(${CMAKE_CURRENT_LIST_DIR}/config/dcd/phydcd/usb_phydcd_config.h "" middleware_usb_phydcd_config_header)
    
  

