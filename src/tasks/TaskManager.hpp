#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "TaskTable.hpp"
#include "StaticTasksTable.hpp"
#include <vector>
#include <cstdio>

class TaskManager
{
public:
	static std::vector<TasksTable> &GetTaskList()
	{
		static std::vector<TasksTable> tasks;
		return tasks;
	}

	// 加载静态任务表
	static void LoadFromTable()
	{
		size_t count;
		const StaticTaskEntry *table = GetStaticTaskTable(count);

		for (size_t i = 0; i < count; ++i) {
			Tasks *taskObj = table[i].factory();
			GetTaskList().emplace_back(taskObj, table[i].name, table[i].stackSize, table[i].priority, table[i].userParam);
		}
	}

	static void TaskEntry(void *param)
	{
		TasksTable *entry = static_cast<TasksTable *>(param);

		if (entry && entry->task) {
			entry->task->CallBack(entry->userParam);
		}

		vTaskDelete(nullptr);
	}

	static bool InitAllTasks()
	{
		auto &tasks = GetTaskList();

		for (auto &t : tasks) {
			if (xTaskCreate(TaskEntry, t.name, t.stackSize, &t, t.priority, &t.taskHandle) != pdPASS) {
				printf("Task %s creation failed!\r\n", t.name);
				return false;
			}
		}

		return true;
	}
};

#endif

