#!/bin/sh

# 若 build 目录已存在，直接进入；否则创建
# 创建或清空 build 目录
mkdir -p build
rm -rf build/* build/.* 2>/dev/null || true  # 注意：防止误删 . 或 ..

cd build

# 清理旧的 CMake 构建产物
# [ -d "CMakeFiles" ] && rm -rf CMakeFiles
# [ -f "Makefile" ] && rm -f Makefile
# [ -f "build.ninja" ] && rm -f build.ninja
# [ -f "cmake_install.cmake" ] && rm -f cmake_install.cmake
# [ -f "CMakeCache.txt" ] && rm -f CMakeCache.txt

# 指定源码目录为上一级（..），因为 build/ 不包含 CMakeLists.txt
cmake -DCMAKE_TOOLCHAIN_FILE="../tools/cmake_toolchain_files/armgcc.cmake" \
      -G "Unix Makefiles" \
      -DCMAKE_BUILD_TYPE=flexspi_nor_debug \
      ..

# 构建项目，并输出到日志
make -j 2>&1 | tee build_log.txt
