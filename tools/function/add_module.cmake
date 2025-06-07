function(add_module)
    set(options)
    set(oneValueArgs MODULE)
    set(multiValueArgs SRCS INC SUBDIRECTORY)
    cmake_parse_arguments(MOD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # === 源文件收集 ===
    foreach(pattern ${MOD_SRCS})
        file(GLOB LOCAL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${pattern}")
        list(APPEND ALL_LOCAL_SRC ${LOCAL_SRC})
    endforeach()

    # === 包含路径收集 ===
    foreach(inc ${MOD_INC})
        list(APPEND ALL_LOCAL_INC ${inc})
    endforeach()

    # === 子目录递归调用 add_module() ===
    foreach(sub ${MOD_SUBDIRECTORY})
        add_subdirectory(${sub})
    endforeach()

    # === 写入缓存（支持多层级累加） ===
    foreach(f ${ALL_LOCAL_SRC})
        list(APPEND GLOBAL_ALL_SRC_FILES "${f}")
    endforeach()

    foreach(i ${ALL_LOCAL_INC})
        list(APPEND GLOBAL_ALL_INC_DIRS "${i}")
    endforeach()

    # 去重并写入缓存（全局变量）
    list(REMOVE_DUPLICATES GLOBAL_ALL_SRC_FILES)
    list(REMOVE_DUPLICATES GLOBAL_ALL_INC_DIRS)

    set(GLOBAL_ALL_SRC_FILES "${GLOBAL_ALL_SRC_FILES}" CACHE INTERNAL "")
    set(GLOBAL_ALL_INC_DIRS "${GLOBAL_ALL_INC_DIRS}" CACHE INTERNAL "")
endfunction()