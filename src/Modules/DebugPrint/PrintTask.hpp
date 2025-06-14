#ifndef PRINT_TASK_HPP
#define PRINT_TASK_HPP

#include "Tasks.hpp"
#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"
#include "Uart.h"
#include "Ringbuffer.hpp"

class PrintTaskt : public Tasks
{
public:
	PrintTaskt() = default;
	~PrintTaskt() override = default;

	Uart uart;
	Ringbuffer _ringbuffer;
	void Init(void *handle) override
	{
		// uart.Init();
		_ringbuffer.allocate(4096);
		// 可选初始化逻辑
	}

	void CallBack(void *param) override
	{
		while (1) {
			size_t space_before = _ringbuffer.space_available();

			uint8_t test[3] = {1, 2, 3};

// 写入测试数据到 ringbuffer
			_ringbuffer.push_back(test, sizeof(test));

			size_t space_used_after_write = _ringbuffer.space_used();

			usb_echo("_ringbuffer space_available=%d, space_used=%d (after write)\r\n",
				 (int)space_before, (int)space_used_after_write);

// 准备读取缓冲区数据
			uint8_t readback[3] = {0};
			_ringbuffer.pop_front(readback, sizeof(readback));

			size_t space_used_after_read = _ringbuffer.space_used();

			usb_echo("_ringbuffer space_used=%d (after read), data=[%d, %d, %d]\r\n",
				 (int)space_used_after_read,
				 readback[0], readback[1], readback[2]);


			const char *name = pcTaskGetName(NULL);
			usb_echo("Task [%s] running, param: %p\r\n", name, param);
			vTaskDelay(pdMS_TO_TICKS(1000));  // 更标准写法
		}
	}
};


#endif
