#ifndef PRINT_TASK_HPP
#define PRINT_TASK_HPP

#include "Tasks.hpp"
#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"

class PrintTaskt : public Tasks {
public:
    PrintTaskt() = default;
    ~PrintTaskt() override = default;

    void Init(void *handle) override {
        // 可选初始化逻辑
    }

    void CallBack(void *param) override {
        while (1) {
            const char *name = pcTaskGetName(NULL);
            usb_echo("Task [%s] running, param: %p\r\n", name, param);
            vTaskDelay(pdMS_TO_TICKS(1000));  // 更标准写法
        }
    }
};


#endif
