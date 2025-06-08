# RT1064 Firmware Project

本工程基于 NXP i.MX RT1064 平台，使用 CMake + Make 构建系统。支持常规构建、烧录、代码格式化等开发流程。

---

## 📦 项目依赖

- [MCUXpresso SDK](https://mcuxpresso.nxp.com/)
- GNU ARM Embedded Toolchain
- CMake >= 3.13
- arm-none-eabi-gcc/g++ Require >= 10.3.0 (e.g.
  10.3-2021.10).

---

## 🛠️ 可用命令

你可以通过以下 `make` 命令来进行构建、清理、烧录和代码格式化等操作：

| 命令                  | 功能说明                                |
|-----------------------|-----------------------------------------|
| `make`                | 编译项目（默认构建配置）               |
| `make clean`          | 清理构建产物（移除 `build/` 目录）     |
| `make flash`          | 使用 **JLink** 烧录固件到目标板           |
| `make format`         | 自动格式化项目代码（使用 `clang-format`）|
| `make check_format`   | `git commit hook` |

---

## 📁 项目结构简述

<details> <summary>点击展开目录结构</summary>

```text
.
├── board/                         # 板级支持
├── build/                         # 构建输出目录（自动生成）
├── build.sh                       # 构建脚本
├── clean.sh                       # 清理脚本
├── CMakeLists.txt                 # 顶层 CMake 构建脚本
├── CMSIS/                         # ARM CMSIS 标准库
├── components/                    # 第三方组件或驱动
├── config.cmake                   # 项目配置文件
├── devices/                       # NXP SoC 相关外设驱动
├── docs/                          # 项目文档
├── flags.cmake                    # 编译选项配置
├── jlink-flash-fw-standalone.jlink  # JLink 烧录脚本
├── Makefile                       # 构建入口
├── middleware/                    # 协议栈、中间件
├── MIMXRT1064xxxxx_flexspi_nor.ld # Flash 链接脚本
├── MIMXRT1064xxxxx_ram.ld         # RAM 链接脚本
├── MIMXRT1064xxxxx_sdram.ld       # SDRAM 链接脚本
├── readme.md                      # 项目说明文档
├── rtos/                          # FreeRTOS / 操作系统封装
├── src/                           # 应用源代码
└── tools/                         # 工具链文件与脚本
```
</details>

## 🔗 参考文档

请参考 [MCUXpresso SDK Documentation](https://mcuxpresso.nxp.com/mcuxsdk/25.03.00) 以获取更详细的 SDK 说明与配置方法。

---