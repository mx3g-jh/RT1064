/*
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdmmc_common.h"
#include "microseconds.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
SDK_ALIGN(uint32_t g_sdmmc[SDK_SIZEALIGN(SDMMC_GLOBAL_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
/*******************************************************************************
 * Code
 ******************************************************************************/
status_t SDMMC_SelectCard(SDMMCHOST_TYPE *base,
                          SDMMCHOST_TRANSFER_FUNCTION transfer,
                          uint32_t relativeAddress,
                          bool isSelected)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_SelectCard;
    if (isSelected)
    {
        command.argument = relativeAddress << 16U;
        command.responseType = kCARD_ResponseTypeR1;
    }
    else
    {
        command.argument = 0U;
        command.responseType = kCARD_ResponseTypeNone;
    }

    content.command = &command;
    content.data = NULL;
    if ((kStatus_Success != transfer(base, &content)) || (command.response[0U] & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Wait until card to transfer state */
    return kStatus_Success;
}

status_t SDMMC_SendApplicationCommand(SDMMCHOST_TYPE *base,
                                      SDMMCHOST_TRANSFER_FUNCTION transfer,
                                      uint32_t relativeAddress)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_ApplicationCommand;
    command.argument = (relativeAddress << 16U);
    command.responseType = kCARD_ResponseTypeR1;

    content.command = &command;
    content.data = 0U;
    if ((kStatus_Success != transfer(base, &content)) || (command.response[0U] & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    if (!(command.response[0U] & kSDMMC_R1ApplicationCommandFlag))
    {
        return kStatus_SDMMC_CardNotSupport;
    }

    return kStatus_Success;
}

status_t SDMMC_SetBlockCount(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer, uint32_t blockCount)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_SetBlockCount;
    command.argument = blockCount;
    command.responseType = kCARD_ResponseTypeR1;

    content.command = &command;
    content.data = 0U;
    if ((kStatus_Success != transfer(base, &content)) || (command.response[0U] & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMC_GoIdle(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_GoIdleState;

    content.command = &command;
    content.data = 0U;
    if (kStatus_Success != transfer(base, &content))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMC_GoPreIdle(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_GoIdleState;
    command.argument = 0xF0F0F0F0;

    content.command = &command;
    content.data = 0U;
    if (kStatus_Success != transfer(base, &content))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMC_SetBlockSize(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer, uint32_t blockSize)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_SetBlockLength;
    command.argument = blockSize;
    command.responseType = kCARD_ResponseTypeR1;

    content.command = &command;
    content.data = 0U;
    if ((kStatus_Success != transfer(base, &content)) || (command.response[0U] & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMC_SetCardInactive(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_GoInactiveState;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeNone;

    content.command = &command;
    content.data = 0U;
    if ((kStatus_Success != transfer(base, &content)))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMC_SwitchVoltage(SDMMCHOST_TYPE *base, SDMMCHOST_TRANSFER_FUNCTION transfer)
{
    assert(transfer);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSD_VoltageSwitch;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != transfer(base, &content))
    {
        return kStatus_SDMMC_TransferFailed;
    }
    /* disable card clock */
    SDMMCHOST_ENABLE_CARD_CLOCK(base, false);

    /* check data line and cmd line status */
    if ((GET_SDMMCHOST_STATUS(base) &
         (CARD_DATA1_STATUS_MASK | CARD_DATA2_STATUS_MASK | CARD_DATA3_STATUS_MASK | CARD_DATA0_NOT_BUSY)) != 0U)
    {
        return kStatus_SDMMC_SwitchVoltageFail;
    }

    /* host switch to 1.8V */
    SDMMCHOST_SWITCH_VOLTAGE180V(base, true);

    /* delay 5ms for 1.8V output stable*/
    microseconds_delay(5 * 1000U);

    /*enable sd clock*/
    SDMMCHOST_ENABLE_CARD_CLOCK(base, true);
    /*enable force clock on*/
    SDMMCHOST_FORCE_SDCLOCK_ON(base, true);
    /* delay 1ms*/
    microseconds_delay(1 * 1000U);

    /*disable force clock on*/
    SDMMCHOST_FORCE_SDCLOCK_ON(base, false);

    /* check data line and cmd line status */
    if ((GET_SDMMCHOST_STATUS(base) &
         (CARD_DATA1_STATUS_MASK | CARD_DATA2_STATUS_MASK | CARD_DATA3_STATUS_MASK | CARD_DATA0_NOT_BUSY)) == 0U)
    {
        return kStatus_SDMMC_SwitchVoltageFail;
    }

    return kStatus_Success;
}

status_t SDMMC_ExecuteTuning(SDMMCHOST_TYPE *base,
                             SDMMCHOST_TRANSFER_FUNCTION transfer,
                             uint32_t tuningCmd,
                             uint32_t blockSize,
                             uint32_t tuningStart,
                             uint32_t tuningStep)
{
    SDMMCHOST_TRANSFER content = { 0U };
    SDMMCHOST_COMMAND command = { 0U };
    SDMMCHOST_DATA data = { 0U };
    uint32_t buffer[32U] = { 0U };
    bool tuningError = true;

    command.index = tuningCmd;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize = blockSize;
    data.blockCount = 1U;
    data.rxData = buffer;
    /* add this macro for adpter to different driver */
    SDMMCHOST_ENABLE_TUNING_FLAG(data);

    content.command = &command;
    content.data = &data;

    /* enable the standard tuning */
    SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, tuningStart, tuningStep, true);

    while (true)
    {
        /* send tuning block */
        if ((kStatus_Success != transfer(base, &content)))
        {
            return kStatus_SDMMC_TransferFailed;
        }
        // SDMMCHOST_Delay(1U);

        /*wait excute tuning bit clear*/
        if ((SDMMCHOST_EXECUTE_STANDARD_TUNING_STATUS(base) != 0U))
        {
            continue;
        }

        /* if tuning error , re-tuning again */
        if ((SDMMCHOST_CHECK_TUNING_ERROR(base) != 0U) && tuningError)
        {
            tuningError = false;
            /* enable the standard tuning */
            SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, tuningStart, tuningStep, true);
            SDMMCHOST_ADJUST_TUNING_DELAY(base, SDMMCHOST_STANDARD_TUNING_START);
        }
        else
        {
            break;
        }
    }

    /* delay to wait the host controller stable */
    // SDMMCHOST_Delay(100U);

    /* check tuning result*/
    if (SDMMCHOST_EXECUTE_STANDARD_TUNING_RESULT(base) == 0U)
    {
        return kStatus_SDMMC_TuningFail;
    }

    SDMMCHOST_AUTO_STANDARD_RETUNING_TIMER(base);

    return kStatus_Success;
}
