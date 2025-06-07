OPENOCD		?= JLinkExe
MAKE_ARGS = -j14
USE_MCU_NAME = MIMXRT1064xxx6B
# === 项目配置 ===
BUILD_DIR := build
TOOLCHAIN_FILE := tools/cmake_toolchain_files/armgcc.cmake
BUILD_TYPE := flexspi_nor_debug
BUILD_NAME := RT1064
GENERATOR := Unix Makefiles
LOG := build_log.txt

.PHONY: all clean config build

# 默认目标：清理 → 配置 → 构建
all:
	@$(MAKE) clean
	$(call generate-jlink-script)
	@$(MAKE) config
	@$(MAKE) build

# 清空 build 目录内容（不删除 build 文件夹本身）
clean:
	@echo "==> Cleaning build directory..."
	mkdir -p $(BUILD_DIR)
	rm -rf $(BUILD_DIR)/* $(BUILD_DIR)/.* 2>/dev/null || true

# 运行 CMake 配置
config:
	@echo "==> Configuring with CMake..."
	cd $(BUILD_DIR) && cmake -DCMAKE_TOOLCHAIN_FILE=../$(TOOLCHAIN_FILE) \
		-G "$(GENERATOR)" \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_BUILD_NAME=$(BUILD_NAME) \
		..

# 编译并输出日志
build:
	@echo "==> Building with $(MAKE_ARGS)..."
	$(MAKE) -C $(BUILD_DIR) $(MAKE_ARGS) | tee $(BUILD_DIR)/$(LOG)

# rebuild jlink-flash-fw-standalone.jlink
define generate-jlink-script
	@rm -f jlink-flash-fw-standalone.jlink
	@echo "Creating jlink-flash-fw-standalone.jlink"
	@echo "h" >> jlink-flash-fw-standalone.jlink
	@echo "loadfile ./build/${BUILD_NAME}.bin, 0x070000000" >> jlink-flash-fw-standalone.jlink
	@echo "r" >> jlink-flash-fw-standalone.jlink
	@echo "q" >> jlink-flash-fw-standalone.jlink
endef

flash:
	@$(MAKE) all
	@if [ -f ./build/${BUILD_NAME}.bin ]; then \
		$(OPENOCD) -device $(USE_MCU_NAME) -speed 4000 -if SWD -CommanderScript jlink-flash-fw-standalone.jlink; \
		echo "$(OPENOCD) flash done !"; \
	else \
		echo "Error: ./build/${BUILD_NAME}.bin not found!"; \
		exit 1; \
	fi

# Makefile for Astyle
# -------------------------
# use dos2unix fix style
# brew install dos2unix
# dos2unix xxx.xx

# Define check_format target
check_format:
	@echo "Checking formatting with astyle"
	@./tools/astyle/check_code_style_all.sh
	@echo "Running git diff to check for changes"
	@git diff --check

# Define format target
format:
	@echo "Formatting with astyle"
	@./tools/astyle/check_code_style_all.sh --fix

.PHONY:
	clean upload check_format format
