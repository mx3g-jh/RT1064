/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "main.h"
#include "TaskManager.hpp"

extern "C" void app_main(void)
{
	TaskManager::LoadFromTable();  // 从静态表加载所有任务

	if (!TaskManager::InitAllTasks()) {
		printf("Some tasks failed to start.\n");
	}
}

#ifdef __cplusplus
extern "C" {
#endif

// 定义内存区域结构体数组
static HeapRegion_t xHeapRegions[] = {
	{ (uint8_t *)0x20200000, 128 * 1024 },  // m_data 区，128KB
	{ (uint8_t *)0x20220000, 128 * 1024 },  // m_data2 区，128KB ： 768KB total
	{ NULL, 0 }                             // 结束标志
};

void ConfigureHeapRegions(void)
{
	vPortDefineHeapRegions(xHeapRegions);
}

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

usb_cdc_vcom_struct_t s_cdcVcom;
static char const *s_appName = "App task";
static char const *s_printName = "Print task";

typedef struct {
	clock_name_t name;
	const char *label;
} ClockNameInfo;

#if defined(__cplusplus)
}

#endif /* __cplusplus */
/*!
 * @brief Application task function.
 *
 * This function runs the task for application.
 *
 * @return None.
 */
void APPTask(void *handle)
{
	usb_status_t error;
	USB_DeviceApplicationInit();

	while (1) {
		if ((1U == s_cdcVcom.attach) && (1U == s_cdcVcom.startTransactions)) {
			const char *taskName = pcTaskGetName(NULL);           // 当前任务名
			TickType_t tick = xTaskGetTickCount();                // 获取当前Tick
			uint32_t ms = tick * portTICK_PERIOD_MS;              // 转换为毫秒

			char msg[128];
			int len = snprintf(msg, sizeof(msg),
					   "[%s] Tick: %lu ms - Hello USB\r\n",
					   taskName, ms);

			error = USB_DeviceCdcAcmSend(
					s_cdcVcom.cdcAcmHandle,
					USB_CDC_VCOM_BULK_IN_ENDPOINT,
					(uint8_t *)msg,
					len);

			if (error != kStatus_USB_Success) {
				// USB send failed
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void print_all_clock_freqs(void)
{
	ClockNameInfo clocks[] = {
		{kCLOCK_CpuClk, "CPU Clock"},
		{kCLOCK_AhbClk, "AHB Clock"},
		{kCLOCK_SemcClk, "SEMC Clock"},
		{kCLOCK_IpgClk, "IPG Clock"},
		{kCLOCK_PerClk, "PER Clock"},
		{kCLOCK_OscClk, "OSC Clock"},
		{kCLOCK_RtcClk, "RTC Clock"},
		{kCLOCK_ArmPllClk, "ARM PLL Clock"},
		{kCLOCK_Usb1PllClk, "USB1 PLL Clock"},
		{kCLOCK_Usb1PllPfd0Clk, "USB1 PLL PFD0"},
		{kCLOCK_Usb1PllPfd1Clk, "USB1 PLL PFD1"},
		{kCLOCK_Usb1PllPfd2Clk, "USB1 PLL PFD2"},
		{kCLOCK_Usb1PllPfd3Clk, "USB1 PLL PFD3"},
		{kCLOCK_Usb1SwClk, "USB1 SW Clock"},
		{kCLOCK_Usb1Sw120MClk, "USB1 SW 120M"},
		{kCLOCK_Usb1Sw60MClk, "USB1 SW 60M"},
		{kCLOCK_Usb1Sw80MClk, "USB1 SW 80M"},
		{kCLOCK_Usb2PllClk, "USB2 PLL Clock"},
		{kCLOCK_SysPllClk, "SYS PLL Clock"},
		{kCLOCK_SysPllPfd0Clk, "SYS PLL PFD0"},
		{kCLOCK_SysPllPfd1Clk, "SYS PLL PFD1"},
		{kCLOCK_SysPllPfd2Clk, "SYS PLL PFD2"},
		{kCLOCK_SysPllPfd3Clk, "SYS PLL PFD3"},
		{kCLOCK_EnetPll0Clk, "ENET PLL0"},
		{kCLOCK_EnetPll1Clk, "ENET PLL1"},
		{kCLOCK_EnetPll2Clk, "ENET PLL2"},
		{kCLOCK_AudioPllClk, "Audio PLL Clock"},
		{kCLOCK_VideoPllClk, "Video PLL Clock"},
	};

	usb_echo("\r\nSystem Clock Frequencies:\r\n");
	usb_echo("--------------------------------------\r\n");

	for (size_t i = 0; i < sizeof(clocks) / sizeof(clocks[0]); ++i) {
		uint32_t freq = CLOCK_GetFreq(clocks[i].name);

		if (freq > 0) {
			usb_echo("%s : %u Hz\r\n", clocks[i].label, freq);

		} else {
			usb_echo("%s : N/A\r\n", clocks[i].label);  // 某些未启用的时钟
		}
	}

	usb_echo("--------------------------------------\r\n\r\n");
}

void PrintTask(void *handle)
{
	char buffer[256];

	while (1) {
		TickType_t t1 = xTaskGetTickCount();
		// 缓冲区大小建议大一点，FreeRTOS 文档推荐至少 512 字节
		usb_echo("============== FreeRTOS System Info ==============\r\n");

// 打印任务列表
		vTaskList(buffer);
		usb_echo("Task List:\r\n");
		usb_echo("Name                  State   Prio     Stack   Num\r\n");
		usb_echo("**************************************************\r\n");
		usb_echo("%s\n", buffer);

// 打印任务运行时间
		vTaskGetRunTimeStats(buffer);
		usb_echo("Task Runtime Stats:\r\n");
		usb_echo("Name                  Time      %%\r\n");
		usb_echo("**************************************************\r\n");
		usb_echo("%s\n", buffer);

// 打印系统其他信息
		usb_echo("System tick: %u\r\n", xTaskGetTickCount());
		TickType_t tick = xTaskGetTickCount();                // 获取当前Tick
		uint32_t ms = tick * portTICK_PERIOD_MS;              // 转换为毫秒
		usb_echo("System time : %u ms\r\n", ms);
		usb_echo("GPT TimerCount : %u\r\n", GPT_GetCurrentTimerCount(GPT1));
		usb_echo("Free heap: %u bytes\r\n", xPortGetFreeHeapSize());
		usb_echo("Min ever free heap : %u bytes\r\n", xPortGetMinimumEverFreeHeapSize());
		usb_echo("==================================================\r\n\r\n");

		print_all_clock_freqs();
		TickType_t t2 = xTaskGetTickCount();
		vTaskDelay(pdMS_TO_TICKS(1000));
		usb_echo("实际间隔Tick = %u\r\n", t2 - t1);
	}
}

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
	BOARD_InitHardware();
	ConfigureHeapRegions();

	if (xTaskCreate(APPTask,                                      /* pointer to the task                      */
			s_appName,                                    /* task name for kernel awareness debugging */
			APP_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), /* task stack size                          */
			&s_cdcVcom,                                   /* optional task startup argument           */
			4,                                            /* initial priority                         */
			&s_cdcVcom.applicationTaskHandle              /* optional task handle to create           */
		       ) != pdPASS) {
		usb_echo("app task create failed! \r\n");
		#if (defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__))
		return 1;
		#else
		return;
		#endif
	}

	if (xTaskCreate(PrintTask,                                      /* pointer to the task                      */
			s_printName,                                    /* task name for kernel awareness debugging */
			APP_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), /* task stack size                          */
			NULL,                                   /* optional task startup argument           */
			4,                                            /* initial priority                         */
			NULL              /* optional task handle to create           */
		       ) != pdPASS) {
		usb_echo("print task create failed!\r\n");
		#if (defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__))
		return 1;
		#else
		return;
		#endif
	}

	app_main();
	vTaskStartScheduler();

	#if (defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__))
	return 1;
	#endif
}

uint32_t ulGetRunTimeCounterValue(void)
{
	return GPT_GetCurrentTimerCount(GPT1);
}

void vConfigureTimerForRunTimeStats(void)
{
	gpt_config_t gptConfig;

	GPT_GetDefaultConfig(&gptConfig);
	gptConfig.clockSource = kGPT_ClockSource_Periph;  // 根据系统实际设置

	// 使能 GPT 时钟
	CLOCK_EnableClock(kCLOCK_Gpt1);  // GPT1 用的是 GPT1 时钟
	GPT_Init(GPT1, &gptConfig);

	// 设置为最大分辨率，不分频
	GPT_SetClockDivider(GPT1, 75);

	// 以最大速度运行，计数到最大
	GPT_StartTimer(GPT1);
}