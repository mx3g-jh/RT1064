#ifndef TASK_TABLE_HPP
#define TASK_TABLE_HPP

#include "Tasks.hpp"
#include "FreeRTOS.h"
#include "task.h"

struct TasksTable {
	Tasks *task;              // 任务对象
	const char *name;         // 名称（调试用）
	uint16_t stackSize;       // 栈大小
	UBaseType_t priority;     // 优先级
	void *userParam;          // 用户传参
	TaskHandle_t taskHandle;  // FreeRTOS 的任务句柄

	TasksTable(Tasks *t, const char *n, uint16_t s, UBaseType_t p, void *param = nullptr)
		: task(t), name(n), stackSize(s), priority(p), userParam(param), taskHandle(nullptr) {}
};

#endif
