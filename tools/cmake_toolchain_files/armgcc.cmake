cmake_minimum_required(VERSION 3.13)

# 设置 CMake 目标系统为裸机和架构
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 工具链扩展
if(WIN32)
    set(TOOLCHAIN_EXT ".exe")
else()
    set(TOOLCHAIN_EXT "")
endif()

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# 工具链三元组
set(TARGET_TRIPLET "arm-none-eabi")

# 先尝试使用环境变量 ARMGCC_DIR
if(DEFINED ENV{ARMGCC_DIR})
    set(TOOLCHAIN_DIR $ENV{ARMGCC_DIR})
    string(REPLACE "\\" "/" TOOLCHAIN_DIR "${TOOLCHAIN_DIR}")
    set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_DIR}/bin)
    set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_DIR}/${TARGET_TRIPLET}/include)
    set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_DIR}/${TARGET_TRIPLET}/lib)

    set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-gcc${TOOLCHAIN_EXT})
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-g++${TOOLCHAIN_EXT})
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-gcc${TOOLCHAIN_EXT})
    set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-objcopy CACHE INTERNAL "objcopy tool")
    set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-objdump CACHE INTERNAL "objdump tool")

    message(STATUS "TOOLCHAIN_DIR from ARMGCC_DIR: ${TOOLCHAIN_DIR}")
else()
    # 回退到 PATH 中的 arm-none-eabi 工具链
    set(TOOLCHAIN_PREFIX arm-none-eabi-)

    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
    set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}objdump CACHE INTERNAL "objdump tool")

    message(WARNING "ARMGCC_DIR not set, using ${TOOLCHAIN_PREFIX} from PATH.")
endif()

# 强制使用指定编译器
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# 设置编译选项
set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "c compiler flags debug")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "cxx compiler flags debug")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm compiler flags debug")

set(CMAKE_C_FLAGS_RELEASE "-O3" CACHE INTERNAL "c compiler flags release")
set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE INTERNAL "cxx compiler flags release")

# 处理 binutils >= 2.39 的 no-warn-rwx-segments 警告
execute_process(COMMAND ${CMAKE_C_COMPILER} -print-prog-name=ld RESULT_VARIABLE RUN_C_RESULT OUTPUT_VARIABLE FULL_LD_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
if(${RUN_C_RESULT} EQUAL 0)
    execute_process(COMMAND ${FULL_LD_PATH} --help RESULT_VARIABLE RUN_LD_RESULT OUTPUT_VARIABLE LD_HELP_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(${RUN_LD_RESULT} EQUAL 0)
        string(FIND "${LD_HELP_OUTPUT}" "no-warn-rwx-segments" LD_RWX_WARNING_SUPPORTED)
        if(${LD_RWX_WARNING_SUPPORTED} GREATER -1)
            add_link_options("-Wl,--no-warn-rwx-segments")
        endif()
    endif()
endif()

# 设置 find 路径，仅在设置了 TOOLCHAIN_DIR 时才使用
if(DEFINED TOOLCHAIN_DIR)
    set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR}/${TARGET_TRIPLET})
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

# 输出构建类型
message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 引入 NXP MCUXpresso SDK 配置
include(${CMAKE_CURRENT_LIST_DIR}/mcux_config.cmake)

# 可选模块配置接口
if(DEFINED LIBRARY_TYPE)
    if(DEFINED LANGUAGE)
        set_library(${LIBRARY_TYPE} ${LANGUAGE})
    endif()
    if(DEFINED DEBUG_CONSOLE)
        set_debug_console(${DEBUG_CONSOLE} ${LIBRARY_TYPE})
    endif()
endif()

if(DEFINED FPU_TYPE AND DEFINED FPU_ABI)
    set_floating_point(${FPU_TYPE} ${FPU_ABI})
endif()
