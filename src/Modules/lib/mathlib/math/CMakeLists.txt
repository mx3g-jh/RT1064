add_module(
    MODULE math
    SRCS
        *.c
        *.cpp
    INC
        ${CMAKE_CURRENT_SOURCE_DIR}
    SUBDIRECTORY
        filter
)