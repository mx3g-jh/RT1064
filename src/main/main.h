/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MAIN_H_
#define _MAIN_H_ 1

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "usb_virtual_com.h"
#include "task.h"
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${header:end}*/
extern usb_cdc_vcom_struct_t s_cdcVcom;

#ifndef APP_TASK_STACK_SIZE
#define APP_TASK_STACK_SIZE 5000L
#endif
/*${function:start}*/
/* @TEST_ANCHOR */
void APPTask(void *handle);
void print_all_clock_freqs(void);
void PrintTask(void *handle);
uint32_t ulGetRunTimeCounterValue(void);
void vConfigureTimerForRunTimeStats(void);
#ifdef __cplusplus
}

#endif

#endif /* _MAIN_H_ */
