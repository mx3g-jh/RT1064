#ifndef TASKS_HPP
#define TASKS_HPP

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
class Tasks
{
public:
	Tasks() = default;
	virtual ~Tasks() = default;

	virtual void Init(void *handle) = 0;
	virtual void CallBack(void *handle) = 0;

	// 通用的工厂模板函数
	template <typename T>
	static Tasks *Create()
	{
		return new T();
	}
};


#endif
