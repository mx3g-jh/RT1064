/** @file mlan_sdio.c
 *
 *  @brief This file provides mlan driver for SDIO
 *
 *  Copyright 2008-2024 NXP
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *
 */

#if defined(SDK_OS_FREE_RTOS)

#include <wmerrno.h>
#include <fsl_os_abstraction.h>
#include <mlan_sdio_api.h>
#include <mlan_main_defs.h>
#include <board.h>
#include <wifi_bt_config.h>
#include <fsl_common.h>
#include <fsl_clock.h>
#include <fsl_sdio.h>
#include <fsl_sdmmc_spec.h>
#include <fsl_usdhc.h>

#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_common.h"
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
#include "fsl_cache.h"
#endif
#endif

#if CONFIG_WIFI_SG_DEBUG
#define wifi_sg_d(...) wmlog("wifi SG", ##__VA_ARGS__)
#else
#define wifi_sg_d(...)
#endif /* ! CONFIG_WIFI_SG_DEBUG */

/* Command port */
#define CMD_PORT_SLCT 0x8000U

#define MLAN_SDIO_BYTE_MODE_MASK 0x80000000U

#define SDIO_CMD_TIMEOUT 2000

#if FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER

static size_t sg_tx_idx, sg_rx_idx;

static sdmmchost_scatter_gather_data_list_t sgDataListtx[SDIO_MP_AGGR_DEF_PKT_LIMIT_MAX];
static sdmmchost_scatter_gather_data_list_t sgDataListrx[SDIO_MP_AGGR_DEF_PKT_LIMIT_MAX];

void sg_rx_init_table()
{
    memset(&sgDataListrx, 0, sizeof(sgDataListrx));
    sg_rx_idx = 0;
}

void sg_rx_set_buf(uint32_t *buf, size_t len)
{
    if (sg_rx_idx > SDIO_MP_AGGR_DEF_PKT_LIMIT_MAX)
    {
       return;
    }

    sgDataListrx[sg_rx_idx].dataAddr = buf;
    sgDataListrx[sg_rx_idx].dataSize = len;
    if (sg_rx_idx > 0)
    {
        sgDataListrx[sg_rx_idx - 1].dataList = &sgDataListrx[sg_rx_idx];
    }

    sg_rx_idx++;
}

void sg_tx_init_table()
{
    memset(&sgDataListtx, 0, sizeof(sgDataListtx));
    sg_tx_idx = 0;
}

void sg_tx_set_buf(uint32_t *buf, size_t len)
{
    if (sg_tx_idx > SDIO_MP_AGGR_DEF_PKT_LIMIT_MAX)
    {
        return;
    }

    sgDataListtx[sg_tx_idx].dataAddr = buf;
    sgDataListtx[sg_tx_idx].dataSize = len;
    if (sg_tx_idx > 0)
    {
        sgDataListtx[sg_tx_idx - 1].dataList = &sgDataListtx[sg_tx_idx];
    }

    sg_tx_idx++;
}
#endif

extern void handle_cdint(int error);

static sdio_card_t wm_g_sd;
static OSA_MUTEX_HANDLE_DEFINE(sdio_mutex);

int sdio_drv_creg_read(int addr, int fn, uint32_t *resp)
{
    osa_status_t status;

    status = OSA_MutexLock((osa_mutex_handle_t)sdio_mutex, osaWaitForever_c);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("failed to get mutex\r\n");
        return 0;
    }

    if (SDIO_IO_Read_Direct(&wm_g_sd, (sdio_func_num_t)fn, (uint32_t)addr, (uint8_t *)resp) != KOSA_StatusSuccess)
    {
        (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);
        return 0;
    }

    (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);

    return 1;
}

int sdio_drv_creg_write(int addr, int fn, uint8_t data, uint32_t *resp)
{
    osa_status_t status;

    status = OSA_MutexLock((osa_mutex_handle_t)sdio_mutex, osaWaitForever_c);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("failed to get mutex\r\n");
        return 0;
    }

    if (SDIO_IO_Write_Direct(&wm_g_sd, (sdio_func_num_t)fn, (uint32_t)addr, &data, true) != KOSA_StatusSuccess)
    {
        (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);
        return false;
    }

    *resp = data;

    (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);

    return 1;
}

int sdio_drv_read(uint32_t addr, uint32_t fn, uint32_t bcnt, uint32_t bsize, uint8_t *buf, uint32_t *resp)
{
    osa_status_t status;
    uint32_t flags = 0;
    uint32_t param;

    status = OSA_MutexLock((osa_mutex_handle_t)sdio_mutex, osaWaitForever_c);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("failed to get mutex\r\n");
        return 0;
    }

    if (bcnt > 1U)
    {
        flags |= SDIO_EXTEND_CMD_BLOCK_MODE_MASK;
        param = bcnt;
    }
    else
    {
        param = bsize;
    }

    if (SDIO_IO_Read_Extended(&wm_g_sd, (sdio_func_num_t)fn, addr, buf, param, flags) != KOSA_StatusSuccess)
    {
        (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);
        return 0;
    }

    (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);

    return 1;
}

#if FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER
int sdio_drv_read_mb(uint32_t addr, uint32_t fn, uint32_t bcnt, uint32_t bsize)
{
    osa_status_t status;
    uint32_t flags = 0;
    uint32_t param;

    if (sg_rx_idx == 0)
    {
        return 1;
    }

    status = OSA_MutexLock((osa_mutex_handle_t)sdio_mutex, osaWaitForever_c);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("failed to get mutex\r\n");
        return 0;
    }

    if (bcnt > 1U)
    {
        flags |= SDIO_EXTEND_CMD_BLOCK_MODE_MASK;
        param = bcnt;
    }
    else
    {
        param = bsize;
    }

    if (SDIO_IO_Read_Extended_Scatter_Gather(&wm_g_sd, (sdio_func_num_t)fn, addr, sgDataListrx, param, flags) != KOSA_StatusSuccess)
    {
        (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);
        return 0;
    }

    (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);

#if CONFIG_WIFI_SG_DEBUG
    wifi_sg_d("num rx sg : %d\r\n", sg_rx_idx);
    for (int i = 0; i < sg_rx_idx; i++)
    {
        dump_hex(sgDataListrx[i].dataAddr, sgDataListrx[i].dataSize);
    }
#endif

    return 1;
}
#endif

int sdio_drv_write(uint32_t addr, uint32_t fn, uint32_t bcnt, uint32_t bsize, uint8_t *buf, uint32_t *resp)
{
    osa_status_t status;
    uint32_t flags = 0;
    uint32_t param;

    status = OSA_MutexLock((osa_mutex_handle_t)sdio_mutex, osaWaitForever_c);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("failed to get mutex\r\n");
        return 0;
    }

    if (bcnt > 1U)
    {
        flags |= SDIO_EXTEND_CMD_BLOCK_MODE_MASK;
        param = bcnt;
    }
    else
    {
        param = bsize;
    }

    if (SDIO_IO_Write_Extended(&wm_g_sd, (sdio_func_num_t)fn, addr, buf, param, flags) != KOSA_StatusSuccess)
    {
        (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);
        return false;
    }

    (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);

    return 1;
}

#if FSL_USDHC_ENABLE_SCATTER_GATHER_TRANSFER
int sdio_drv_write_mb(uint32_t addr, uint32_t fn, uint32_t bcnt, uint32_t bsize)
{
    osa_status_t status;
    uint32_t flags = 0;
    uint32_t param;

#if CONFIG_WIFI_SG_DEBUG
    wifi_sg_d("num tx sg : %d", sg_tx_idx);
    for (int i = 0; i < sg_tx_idx; i++)
    {
        dump_hex(sgDataListtx[i].dataAddr, sgDataListtx[i].dataSize);
    }
#endif

    status = OSA_MutexLock((osa_mutex_handle_t)sdio_mutex, osaWaitForever_c);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("failed to get mutex\r\n");
        return 0;
    }

    if (bcnt > 1U)
    {
        flags |= SDIO_EXTEND_CMD_BLOCK_MODE_MASK;
        param = bcnt;
    }
    else
    {
        param = bsize;
    }

    if (SDIO_IO_Write_Extended_Scatter_Gather(&wm_g_sd, (sdio_func_num_t)fn, addr, sgDataListtx, param, flags) != KOSA_StatusSuccess)
    {
        (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);
        return 0;
    }

    (void)OSA_MutexUnlock((osa_mutex_handle_t)sdio_mutex);

    return 1;
}
#endif

static void SDIO_CardInterruptCallBack(void *userData)
{
    SDMMCHOST_EnableCardInt(wm_g_sd.host, false);
    handle_cdint(0);
}

void sdio_enable_interrupt(void)
{
    if (wm_g_sd.isHostReady)
    {
        SDMMCHOST_EnableCardInt(wm_g_sd.host, true);
    }
}

void sdio_disable_interrupt(void)
{
    if (wm_g_sd.isHostReady)
    {
        SDMMCHOST_EnableCardInt(wm_g_sd.host, false);
    }
}

static void sdio_controller_init(void)
{
    (void)memset(&wm_g_sd, 0, sizeof(sdio_card_t));

    BOARD_WIFI_BT_Config(&wm_g_sd, SDIO_CardInterruptCallBack);

#if defined(SD_TIMING_MAX)
    wm_g_sd.currentTiming = SD_TIMING_MAX;
#endif
#if defined(SD_CLOCK_MAX)
    wm_g_sd.usrParam.maxFreq = SD_CLOCK_MAX;
#endif
}

static int sdio_card_init(void)
{
    int ret       = WM_SUCCESS;
    uint32_t resp = 0;

    if (SDIO_HostInit(&wm_g_sd) != KOSA_StatusSuccess)
    {
        return kStatus_SDMMC_HostNotReady;
    }

#if defined(SDMMCHOST_OPERATION_VOLTAGE_3V3)
    /* Disable switch to 1.8V in SDIO_ProbeBusVoltage() */
    wm_g_sd.usrParam.ioVoltage = NULL;
#elif defined(SDMMCHOST_OPERATION_VOLTAGE_1V8)
    /* Switch to 1.8V */
    if ((wm_g_sd.usrParam.ioVoltage != NULL) && (wm_g_sd.usrParam.ioVoltage->type == kSD_IOVoltageCtrlByGpio))
    {
        if (wm_g_sd.usrParam.ioVoltage->func != NULL)
        {
            wm_g_sd.usrParam.ioVoltage->func(kSDMMC_OperationVoltage180V);
        }
    }
#if SDMMCHOST_SUPPORT_VOLTAGE_CONTROL
    else if ((wm_g_sd.usrParam.ioVoltage != NULL) && (wm_g_sd.usrParam.ioVoltage->type == kSD_IOVoltageCtrlByHost))
    {
        SDMMCHOST_SwitchToVoltage(wm_g_sd.host, (uint32_t)kSDMMC_OperationVoltage180V);
    }
#endif
    else
    {
        /* Do Nothing */
    }
    wm_g_sd.operationVoltage = kSDMMC_OperationVoltage180V;
#endif

#if !defined(COEX_APP_SUPPORT) || (defined(COEX_APP_SUPPORT) && !(CONFIG_WIFI_IND_DNLD))
    BOARD_WIFI_BT_Enable(true);
#endif

    ret = SDIO_CardInit(&wm_g_sd);
    if (ret != WM_SUCCESS)
    {
        return ret;
    }

    (void)sdio_drv_creg_read(0x0, 0, &resp);

    sdio_d("Card Version - (0x%x)", resp & 0xff);

    /* Mask interrupts in card */
    (void)sdio_drv_creg_write(0x4, 0, 0x3, &resp);
    /* Enable IO in card */
    (void)sdio_drv_creg_write(0x2, 0, 0x2, &resp);

#if defined(SD9177) || defined(SD8978) || defined(IW610)
    (void)SDIO_SetBlockSize(&wm_g_sd, (sdio_func_num_t)0, 1);
#elif defined(SD8801) || defined(SD8987)
    (void)SDIO_SetBlockSize(&wm_g_sd, (sdio_func_num_t)0, 256);
#endif
    (void)SDIO_SetBlockSize(&wm_g_sd, (sdio_func_num_t)1, 256);
    (void)SDIO_SetBlockSize(&wm_g_sd, (sdio_func_num_t)2, 256);

    return ret;
}

static void print_card_info(sdio_card_t *card)
{
    assert(card != NULL);

    if (card->operationVoltage == kSDMMC_OperationVoltage330V)
    {
        sdio_d("Voltage: 3.3V");
    }
    else if (card->operationVoltage == kSDMMC_OperationVoltage180V)
    {
        sdio_d("Voltage: 1.8V");
    }

    if (card->currentTiming == kSD_TimingSDR12DefaultMode)
    {
        if (card->operationVoltage == kSDMMC_OperationVoltage330V)
        {
            sdio_d("Timing mode: Default mode");
        }
        else if (card->operationVoltage == kSDMMC_OperationVoltage180V)
        {
            sdio_d("Timing mode: SDR12 mode");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR25HighSpeedMode)
    {
        if (card->operationVoltage == kSDMMC_OperationVoltage180V)
        {
            sdio_d("Timing mode: SDR25");
        }
        else
        {
            sdio_d("Timing mode: High Speed");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR50Mode)
    {
        sdio_d("Timing mode: SDR50");
    }
    else if (card->currentTiming == kSD_TimingSDR104Mode)
    {
        sdio_d("Timing mode: SDR104");
    }
    else if (card->currentTiming == kSD_TimingDDR50Mode)
    {
        sdio_d("Timing mode: DDR50");
    }
}

int sdio_drv_init(void (*cd_int)(int))
{
    osa_status_t status;

    status = OSA_MutexCreate((osa_mutex_handle_t)sdio_mutex);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("Failed to create mutex");
        return -WM_FAIL;
    }

    sdio_controller_init();

    if (sdio_card_init() != WM_SUCCESS)
    {
        sdio_e("Card initialization failed");
        return -WM_FAIL;
    }
    else
    {
        sdio_d("Card initialization successful");
    }

    print_card_info(&wm_g_sd);

    return WM_SUCCESS;
}

void sdio_drv_deinit(void)
{
    osa_status_t status;

    SDIO_Deinit(&wm_g_sd);

    status = OSA_MutexDestroy((osa_mutex_handle_t)sdio_mutex);
    if (status != KOSA_StatusSuccess)
    {
        sdio_e("Failed to delete mutex");
    }
}

#elif defined(__ZEPHYR__)

#include <mlan_sdio_api.h>
#include <osa.h>
#include <fsl_common.h>
#include <fsl_gpio.h>
#include <zephyr/sd/sdio.h>

#define SDIO_CMD_TIMEOUT 2000

const struct device *sdhc_dev = DEVICE_DT_GET(DT_BUS(DT_COMPAT_GET_ANY_STATUS_OKAY(nxp_wifi)));

static struct sd_card wm_g_sd;
static struct sdio_func g_sdio_funcs[8];

int sdio_drv_creg_read(int addr, int fn, uint32_t *resp)
{
    struct sdio_func *func = &g_sdio_funcs[fn];

    if (sdio_read_byte(func, addr, (uint8_t *)resp) != 0)
    {
        return 0;
    }

    return 1;
}

int sdio_drv_creg_write(int addr, int fn, uint8_t data, uint32_t *resp)
{
    struct sdio_func *func = &g_sdio_funcs[fn];

    if (sdio_rw_byte(func, addr, data, (uint8_t *)resp) != 0)
    {
        return 0;
    }

    return 1;
}

int sdio_drv_read(uint32_t addr, uint32_t fn, uint32_t bcnt, uint32_t bsize, uint8_t *buf, uint32_t *resp)
{
    struct sdio_func *func = &g_sdio_funcs[fn];

    if (sdio_read_addr(func, addr, buf, bcnt * bsize) != 0)
    {
        return 0;
    }

    return 1;
}

int sdio_drv_write(uint32_t addr, uint32_t fn, uint32_t bcnt, uint32_t bsize, uint8_t *buf, uint32_t *resp)
{
    struct sdio_func *func = &g_sdio_funcs[fn];

    if (sdio_write_addr(func, addr, buf, bcnt * bsize) != 0)
    {
        return 0;
    }

    return 1;
}

extern void handle_cdint(int error);

void sdio_irq_handler(const struct device *dev, int reason, const void *user_data)
{
    if (reason == SDHC_INT_SDIO)
    {
        sdhc_disable_interrupt(sdhc_dev, SDHC_INT_SDIO);
        handle_cdint(0);
    }
}

void sdio_enable_interrupt(void)
{
    sdhc_enable_interrupt(sdhc_dev, (sdhc_interrupt_cb_t)sdio_irq_handler, SDHC_INT_SDIO, NULL);
    return;
}

static void sdio_controller_init(void)
{
    (void)memset(&wm_g_sd, 0, sizeof(struct sd_card));
}

static int sdio_card_init(void)
{
    int ret = WM_SUCCESS;
    uint32_t resp;

    if (!device_is_ready(sdhc_dev))
    {
        sdio_e("SD controller not ready");
        return -EIO;
    }

    if (!sdhc_card_present(sdhc_dev))
    {
        sdio_e("SDIO card not present");
        return -EIO;
    }

    ret = sd_init(sdhc_dev, &wm_g_sd);
    if (ret)
    {
        return ret;
    }

    memcpy(&g_sdio_funcs[0], &wm_g_sd.func0, sizeof(struct sdio_func));
    (void)sdio_drv_creg_read(0x0, 0, &resp);

    sdio_d("Card Version - (0x%x)", resp & 0xff);
    /* Init SDIO functions */
    sdio_init_func(&wm_g_sd, &g_sdio_funcs[1], SDIO_FUNC_NUM_1);
    sdio_init_func(&wm_g_sd, &g_sdio_funcs[2], SDIO_FUNC_NUM_2);

    /* Mask interrupts in card */
    (void)sdio_drv_creg_write(0x4, 0, 0x3, &resp);
    /* Enable IO in card */
    (void)sdio_drv_creg_write(0x2, 0, 0x2, &resp);

    (void)sdio_set_block_size(&g_sdio_funcs[0], 256);
    (void)sdio_set_block_size(&g_sdio_funcs[1], 256);
    (void)sdio_set_block_size(&g_sdio_funcs[2], 256);

    return ret;
}

int sdio_drv_init(void (*cd_int)(int))
{
    sdio_controller_init();

    if (sdio_card_init() != WM_SUCCESS)
    {
        sdio_e("Card initialization failed");
        return -WM_FAIL;
    }
    else
    {
        sdio_d("Card initialization successful");
    }

    return WM_SUCCESS;
}

void sdio_drv_deinit(void)
{
    // SDIO_Deinit(&wm_g_sd);
}

#endif
