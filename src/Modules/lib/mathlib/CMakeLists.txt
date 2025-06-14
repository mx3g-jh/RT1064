add_module(
    MODULE mathlib
    SRCS
        *.c
        *.cpp
    INC
        ${CMAKE_CURRENT_SOURCE_DIR}
    SUBDIRECTORY
        math
)