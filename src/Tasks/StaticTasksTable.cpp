#include "StaticTasksTable.hpp"
#include "PrintTask.hpp"

// 静态任务注册表
StaticTaskEntry static_task_table[] = {
	{Tasks::Create<PrintTaskt>, "Print1", 256, 4, (void *)0x1234},
	{Tasks::Create<PrintTaskt>, "Print2", 256, 4, (void *)0x4444},
};


constexpr size_t static_task_count = sizeof(static_task_table) / sizeof(static_task_table[0]);

// 提供接口供 TaskManager 使用
extern "C" const StaticTaskEntry *GetStaticTaskTable(size_t &count)
{
	count = static_task_count;
	return static_task_table;
}
