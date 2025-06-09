#ifndef STATIC_TASKS_TABLE_HPP
#define STATIC_TASKS_TABLE_HPP

#include "Tasks.hpp"
#include <cstdio>
// 任务构造函数函数指针类型（返回 Tasks*）
typedef Tasks *(*TaskFactoryFunc)();

// 静态表定义结构
struct StaticTaskEntry {
	TaskFactoryFunc factory;
	const char *name;
	uint16_t stackSize;
	UBaseType_t priority;
	void *userParam;
};

#ifdef __cplusplus
extern "C" {
#endif

const StaticTaskEntry *GetStaticTaskTable(size_t &count);

#ifdef __cplusplus
}

#endif
#endif
