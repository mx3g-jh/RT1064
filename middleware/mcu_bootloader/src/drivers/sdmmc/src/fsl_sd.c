/*
 * Copyright 2016-2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sd.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Send SELECT_CARD command to set the card to be transfer state or not.
 *
 * @param card Card descriptor.
 * @param isSelected True to set the card into transfer state.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_SelectCard(sd_card_t *card, bool isSelected);

/*!
 * @brief Wait write process complete.
 *
 * @param card Card descriptor.
 * @retval kStatus_Timeout Send command timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_WaitWriteComplete(sd_card_t *card);

/*!
 * @brief Send SEND_APPLICATION_COMMAND command.
 *
 * @param card Card descriptor.
 * @param relativeaddress
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_SendApplicationCmd(sd_card_t *card, uint32_t relativeAddress);

/*!
 * @brief Send GO_IDLE command to set the card to be idle state.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_GoIdle(sd_card_t *card);

/*!
 * @brief Send STOP_TRANSMISSION command after multiple blocks read/write.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_StopTransmission(sd_card_t *card);

/*!
 * @brief Send SET_BLOCK_SIZE command.
 *
 * @param card Card descriptor.
 * @param blockSize Block size.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_SetBlockSize(sd_card_t *card, uint32_t blockSize);

/*!
 * @brief Send GET_RCA command to get card relative address.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendRca(sd_card_t *card);

/*!
 * @brief Send SWITCH_FUNCTION command to switch the card function group.
 *
 * @param card Card descriptor.
 * @param mode 0 to check function group. 1 to switch function group
 * @param group Function group
 * @param number Function number in the function group.
 * @param status Switch function status.
 * @retval kStatus_SDMMC_SetCardBlockSizeFailed Set card block size failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SwitchFunction(sd_card_t *card, uint32_t mode, uint32_t group, uint32_t number, uint32_t *status);

/*!
 * @brief Decode raw SCR register content in the data blocks.
 *
 * @param card Card descriptor.
 * @param rawScr Raw SCR register content.
 */
static void SD_DecodeScr(sd_card_t *card, uint32_t *rawScr);

/*!
 * @brief Send GET_SCR command.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed Send application command failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_NotSupportYet Not support yet.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendScr(sd_card_t *card);

/*!
 * @brief Switch the card to be high speed mode.
 *
 * @param card Card descriptor.
 * @param group Group number.
 * @param functio Function number.
 * @retval kStatus_SDMMC_CardNotSupport Card not support.
 * @retval kStatus_SDMMC_SwitchFailed Switch failed.
 * @retval kStatus_SDMMC_NotSupportYet Not support yet.
 * @retval kStatus_Fail Switch failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SelectFunction(sd_card_t *card, uint32_t group, uint32_t function);

/*!
 * @brief Send SET_DATA_WIDTH command to set SD bus width.
 *
 * @param card Card descriptor.
 * @param width Data bus width.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed Send application command failed.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SetDataBusWidth(sd_card_t *card, sd_data_bus_width_t width);

/*!
 * @brief Decode raw CSD register content in the data blocks.
 *
 * @param card Card descriptor.
 * @param rawCsd Raw CSD register content.
 */
static void SD_DecodeCsd(sd_card_t *card, uint32_t *rawCsd);

/*!
 * @brief Send SEND_CSD command to get CSD register content from Card.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendCsd(sd_card_t *card);

/*!
 * @brief Decode raw CID register content in the data blocks.
 *
 * @param rawCid raw CID register content.
 * @param card Card descriptor.
 */
static void SD_DecodeCid(sd_card_t *card, uint32_t *rawCid);

/*!
 * @brief Send GET_CID command to get CID from card.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_AllSendCid(sd_card_t *card);

/*!
 * @brief Send SEND_OPERATION_CONDITION command.
 *
 * This function sends host capacity support information and asks the accessed card to send its operating condition
 * register content.
 *
 * @param card Card descriptor.
 * @param argument The argument of the send operation condition ncomamnd.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed Send application command failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Timeout Timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_ApplicationSendOperationCondition(sd_card_t *card, uint32_t argument);

/*!
 * @brief Send GET_INTERFACE_CONDITION command to get card interface condition.
 *
 * This function checks card interface condition, which includes host supply voltage information and asks the card
 * whether card supports the specified host voltage.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendInterfaceCondition(sd_card_t *card);

/*!
 * @brief Send switch voltage command
 * switch card voltage to 1.8v
 *
 * @param card Card descriptor.
 */
static status_t SD_SwitchVoltage(sd_card_t *card);

/*!
 * @brief select bus timing
 * select card timing
 * @param card Card descriptor.
 */
static status_t SD_SelectBusTiming(sd_card_t *card);

/*!
 * @brief select card driver strength
 * select card driver strength
 * @param card Card descriptor.
 * @param driverStrength Driver strength
 */
static status_t SD_SetDriverStrength(sd_card_t *card, sd_driver_strength_t driverStrength);

/*!
 * @brief select max current
 * select max operation current
 * @param card Card descriptor.
 * @param maxCurrent Max current
 */
static status_t SD_SetMaxCurrent(sd_card_t *card, sd_max_current_t maxCurrent);

/*!
 * @brief Read data from specific SD card.
 *
 * @param card Card descriptor.
 * @param buffer Buffer to save data blocks read.
 * @param startBlock Card start block number to be read.
 * @param blockSize Block size.
 * @param blockCount Block count.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Wait write complete failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_Read(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount);

/*!
 * @brief Write data to specific card
 *
 * @param card Card descriptor.
 * @param buffer Buffer to be sent.
 * @param startBlock Card start block number to be written.
 * @param blockSize Block size.
 * @param blockCount Block count.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_Write(
    sd_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount);

/*!
 * @brief Erase data for the given block range.
 *
 * @param card Card descriptor.
 * @param startBlock Card start block number to be erased.
 * @param blockCount The block count to be erased.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_Erase(sd_card_t *card, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief card transfer function.
 *
 * @param card Card descriptor.
 * @param content Transfer content.
 * @param retry Retry times
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 * @retval kStatus_SDMMC_TuningFail tuning fail
 */
static status_t SD_Transfer(sd_card_t *card, SDMMCHOST_TRANSFER *content, uint32_t retry);

/*!
 * @brief card execute tuning function.
 *
 * @param card Card descriptor.
 * @retval kStatus_Success Operate successfully.
 * @retval kStatus_SDMMC_TuningFail tuning fail.
 * @retval kStatus_SDMMC_TransferFailed transfer fail
 */
static status_t inline SD_ExecuteTuning(sd_card_t *card);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* g_sdmmc statement */
extern uint32_t g_sdmmc[SDK_SIZEALIGN(SDMMC_GLOBAL_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)];
/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t inline SD_SelectCard(sd_card_t *card, bool isSelected)
{
    assert(card);

    return SDMMC_SelectCard(card->host.base, card->host.transfer, card->relativeAddress, isSelected);
}

static status_t inline SD_SendApplicationCmd(sd_card_t *card, uint32_t relativeAddress)
{
    assert(card);

    return SDMMC_SendApplicationCommand(card->host.base, card->host.transfer, relativeAddress);
}

static status_t inline SD_GoIdle(sd_card_t *card)
{
    assert(card);

    return SDMMC_GoIdle(card->host.base, card->host.transfer);
}

static status_t inline SD_SetBlockSize(sd_card_t *card, uint32_t blockSize)
{
    assert(card);

    return SDMMC_SetBlockSize(card->host.base, card->host.transfer, blockSize);
}

static status_t inline SD_ExecuteTuning(sd_card_t *card)
{
    assert(card);

    return SDMMC_ExecuteTuning(card->host.base, card->host.transfer, kSD_SendTuningBlock, 64U,
                               card->userConfig.tuningStart, card->userConfig.tuningStep);
}

static status_t SD_SwitchVoltage(sd_card_t *card)
{
    assert(card);

    return SDMMC_SwitchVoltage(card->host.base, card->host.transfer);
}

static status_t SD_Transfer(sd_card_t *card, SDMMCHOST_TRANSFER *content, uint32_t retry)
{
    assert(card->host.transfer);
    assert(content);
    status_t error;

    do
    {
        error = card->host.transfer(card->host.base, content);
        if (((error == SDMMCHOST_RETUNING_REQUEST) || (error == SDMMCHOST_TUNING_ERROR) ||
             (content->command->response[0U] & kSDMMC_R1ErrorAllFlag)) &&
            ((card->currentTiming == kSD_TimingSDR104Mode) || (card->currentTiming == kSD_TimingSDR50Mode)))
        {
            /* tuning error need reset tuning circuit */
            if (error == SDMMCHOST_TUNING_ERROR)
            {
                SDMMCHOST_RESET_TUNING(card->host.base, 10000U);
            }

            /* execute re-tuning */
            if (SD_ExecuteTuning(card) != kStatus_Success)
            {
                error = kStatus_SDMMC_TuningFail;
            }
            else
            {
                continue;
            }
        }
        else if (error != kStatus_Success)
        {
            error = kStatus_SDMMC_TransferFailed;
        }

        if (retry != 0U)
        {
            retry--;
        }
        else
        {
            break;
        }

    } while ((error != kStatus_Success) && (error != kStatus_SDMMC_TuningFail));

    return error;
}

static status_t SD_WaitWriteComplete(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_SendStatus;
    command.argument = card->relativeAddress << 16U;
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = kSDMMC_R1ErrorAllFlag;

    do
    {
        content.command = &command;
        content.data = 0U;
        if (kStatus_Success != SD_Transfer(card, &content, 2U))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        if ((command.response[0U] & kSDMMC_R1ReadyForDataFlag) &&
            (SDMMC_R1_CURRENT_STATE(command.response[0U]) != kSDMMC_R1StateProgram))
        {
            break;
        }
    } while (true);

    return kStatus_Success;
}

static status_t SD_StopTransmission(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_StopTransmission;
    command.argument = 0U;
    command.type = kCARD_CommandTypeAbort;
    command.responseType = kCARD_ResponseTypeR1b;
    command.responseErrorFlags = kSDMMC_R1ErrorAllFlag;

    content.command = &command;
    content.data = 0U;
    if (kStatus_Success != SD_Transfer(card, &content, 1U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static status_t SD_SendRca(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSD_SendRelativeAddress;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR6;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success == card->host.transfer(card->host.base, &content))
    {
        card->relativeAddress = (command.response[0U] >> 16U);
        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static status_t SD_SwitchFunction(sd_card_t *card, uint32_t mode, uint32_t group, uint32_t number, uint32_t *status)
{
    assert(card);
    assert(status);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    SDMMCHOST_DATA data = { 0 };

    command.index = kSD_Switch;
    command.argument = (mode << 31U | 0x00FFFFFFU);
    command.argument &= ~((uint32_t)(0xFU) << (group * 4U));
    command.argument |= (number << (group * 4U));
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize = 64U;
    data.blockCount = 1U;
    data.rxData = status;

    content.command = &command;
    content.data = &data;
    if ((kStatus_Success != card->host.transfer(card->host.base, &content)) ||
        ((command.response[0U]) & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static void SD_DecodeScr(sd_card_t *card, uint32_t *rawScr)
{
    assert(card);
    assert(rawScr);

    sd_scr_t *scr;

    scr = &(card->scr);
    scr->scrStructure = (uint8_t)((rawScr[0U] & 0xF0000000U) >> 28U);
    scr->sdSpecification = (uint8_t)((rawScr[0U] & 0xF000000U) >> 24U);
    if ((uint8_t)((rawScr[0U] & 0x800000U) >> 23U))
    {
        scr->flags |= kSD_ScrDataStatusAfterErase;
    }
    scr->sdSecurity = (uint8_t)((rawScr[0U] & 0x700000U) >> 20U);
    scr->sdBusWidths = (uint8_t)((rawScr[0U] & 0xF0000U) >> 16U);
    if ((uint8_t)((rawScr[0U] & 0x8000U) >> 15U))
    {
        scr->flags |= kSD_ScrSdSpecification3;
    }
    scr->extendedSecurity = (uint8_t)((rawScr[0U] & 0x7800U) >> 10U);
    scr->commandSupport = (uint8_t)(rawScr[0U] & 0x3U);
    scr->reservedForManufacturer = rawScr[1U];
    /* Get specification version. */
    switch (scr->sdSpecification)
    {
        case 0U:
            card->version = kSD_SpecificationVersion1_0;
            break;
        case 1U:
            card->version = kSD_SpecificationVersion1_1;
            break;
        case 2U:
            card->version = kSD_SpecificationVersion2_0;
            if (card->scr.flags & kSD_ScrSdSpecification3)
            {
                card->version = kSD_SpecificationVersion3_0;
            }
            break;
        default:
            break;
    }
    if (card->scr.sdBusWidths & 0x4U)
    {
        card->flags |= kSD_Support4BitWidthFlag;
    }
    /* speed class control cmd */
    if (card->scr.commandSupport & 0x01U)
    {
        card->flags |= kSD_SupportSpeedClassControlCmd;
    }
    /* set block count cmd */
    if (card->scr.commandSupport & 0x02U)
    {
        card->flags |= kSD_SupportSetBlockCountCmd;
    }
}

static status_t SD_SendScr(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    SDMMCHOST_DATA data = { 0 };
    uint32_t *rawScr = g_sdmmc;

    /* memset the global buffer */
    memset(g_sdmmc, 0U, sizeof(g_sdmmc));

    if (kStatus_Success != SD_SendApplicationCmd(card, card->relativeAddress))
    {
        return kStatus_SDMMC_SendApplicationCommandFailed;
    }

    command.index = kSD_ApplicationSendScr;
    command.responseType = kCARD_ResponseTypeR1;
    command.argument = 0U;

    data.blockSize = 8U;
    data.blockCount = 1U;
    data.rxData = rawScr;

    content.data = &data;
    content.command = &command;
    if ((kStatus_Success != card->host.transfer(card->host.base, &content)) ||
        ((command.response[0U]) & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* SCR register data byte sequence from card is big endian(MSB first). */
    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in a
            word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
            card. So the sequence of 4 bytes received in a word should be converted. */
            rawScr[0U] = SWAP_WORD_BYTE_SEQUENCE(rawScr[0U]);
            rawScr[1U] = SWAP_WORD_BYTE_SEQUENCE(rawScr[1U]);
            break;
        case kSDMMCHOST_EndianModeBig:
            break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
        case kSDMMCHOST_EndianModeHalfWordBig:
            rawScr[0U] = SWAP_HALF_WROD_BYTE_SEQUENCE(rawScr[0U]);
            rawScr[1U] = SWAP_HALF_WROD_BYTE_SEQUENCE(rawScr[1U]);
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }
    memcpy(card->rawScr, rawScr, sizeof(card->rawScr));
    SD_DecodeScr(card, rawScr);

    return kStatus_Success;
}

static status_t SD_SelectFunction(sd_card_t *card, uint32_t group, uint32_t function)
{
    assert(card);

    uint32_t *functionStatus = g_sdmmc;
    uint32_t currentFunctionStatus = 0U;

    /* memset the global buffer */
    memset(g_sdmmc, 0, sizeof(g_sdmmc));

    /* check if card support CMD6 */
    if (card->version <= kSD_SpecificationVersion1_0)
    {
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Check if card support high speed mode. */
    if (kStatus_Success != SD_SwitchFunction(card, kSD_SwitchCheck, group, function, functionStatus))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Switch function status byte sequence from card is big endian(MSB first). */
    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in
            a word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
            card. So the sequence of 4 bytes received in a word should be converted. */
            functionStatus[0U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[0U]);
            functionStatus[1U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[1U]);
            functionStatus[2U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[2U]);
            functionStatus[3U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        case kSDMMCHOST_EndianModeBig:
            break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
        case kSDMMCHOST_EndianModeHalfWordBig:
            functionStatus[0U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[0U]);
            functionStatus[1U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[1U]);
            functionStatus[2U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[2U]);
            functionStatus[3U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }
    /* -functionStatus[0U]---bit511~bit480;
       -functionStatus[1U]---bit479~bit448;
       -functionStatus[2U]---bit447~bit416;
       -functionStatus[3U]---bit415~bit384;
       -functionStatus[4U]---bit383~bit352;
       According to the "switch function status[bits 511~0]" return by switch command in mode "check function":
       -Check if function 1(high speed) in function group 1 is supported by checking if bit 401 is set;
       -check if function 1 is ready and can be switched by checking if bits 379~376 equal value 1;
     */
    currentFunctionStatus = ((functionStatus[3U] & 0xFFU) << 8U) | (functionStatus[4U] >> 24U);

    /* check if function is support */
    if (((currentFunctionStatus >> (group * 4U)) & 0xFU) != function)
    {
        return kStatus_SDMMC_NotSupportYet;
    }

    /* Switch to high speed mode. */
    if (kStatus_Success != SD_SwitchFunction(card, kSD_SwitchSet, group, function, functionStatus))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Switch function status byte sequence from card is big endian(MSB first). */
    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* In little endian mode is little endian, SD bus byte transferred first is the byte stored in lowest byte
            position in a word which will cause 4 byte's sequence in a word is not consistent with their original
            sequence from card. So the sequence of 4 bytes received in a word should be converted. */
            functionStatus[3U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        case kSDMMCHOST_EndianModeBig:
            break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
        case kSDMMCHOST_EndianModeHalfWordBig:
            functionStatus[3U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }
    /* According to the "switch function status[bits 511~0]" return by switch command in mode "set function":
       -check if group 1 is successfully changed to function 1 by checking if bits 379~376 equal value 1;
     */
    currentFunctionStatus = ((functionStatus[3U] & 0xFFU) << 8U) | (functionStatus[4U] >> 24U);

    if (((currentFunctionStatus >> (group * 4U)) & 0xFU) != function)
    {
        return kStatus_SDMMC_SwitchFailed;
    }

    return kStatus_Success;
}

static status_t SD_SetDataBusWidth(sd_card_t *card, sd_data_bus_width_t width)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    if (kStatus_Success != SD_SendApplicationCmd(card, card->relativeAddress))
    {
        return kStatus_SDMMC_SendApplicationCommandFailed;
    }

    command.index = kSD_ApplicationSetBusWdith;
    command.responseType = kCARD_ResponseTypeR1;
    switch (width)
    {
        case kSD_DataBusWidth1Bit:
            command.argument = 0U;
            break;
        case kSD_DataBusWidth4Bit:
            command.argument = 2U;
            break;
        default:
            return kStatus_InvalidArgument;
    }

    content.command = &command;
    content.data = NULL;
    if ((kStatus_Success != card->host.transfer(card->host.base, &content)) ||
        ((command.response[0U]) & kSDMMC_R1ErrorAllFlag))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static void SD_DecodeCsd(sd_card_t *card, uint32_t *rawCsd)
{
    assert(card);
    assert(rawCsd);

    sd_csd_t *csd;

    csd = &(card->csd);
    csd->csdStructure = (uint8_t)((rawCsd[3U] & 0xC0000000U) >> 30U);
    csd->dataReadAccessTime1 = (uint8_t)((rawCsd[3U] & 0xFF0000U) >> 16U);
    csd->dataReadAccessTime2 = (uint8_t)((rawCsd[3U] & 0xFF00U) >> 8U);
    csd->transferSpeed = (uint8_t)(rawCsd[3U] & 0xFFU);
    csd->cardCommandClass = (uint16_t)((rawCsd[2U] & 0xFFF00000U) >> 20U);
    csd->readBlockLength = (uint8_t)((rawCsd[2U] & 0xF0000U) >> 16U);
    if (rawCsd[2U] & 0x8000U)
    {
        csd->flags |= kSD_CsdReadBlockPartialFlag;
    }
    if (rawCsd[2U] & 0x4000U)
    {
        csd->flags |= kSD_CsdReadBlockPartialFlag;
    }
    if (rawCsd[2U] & 0x2000U)
    {
        csd->flags |= kSD_CsdReadBlockMisalignFlag;
    }
    if (rawCsd[2U] & 0x1000U)
    {
        csd->flags |= kSD_CsdDsrImplementedFlag;
    }
    switch (csd->csdStructure)
    {
        case 0:
            csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3FFU) << 2U);
            csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xC0000000U) >> 30U);
            csd->readCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x38000000U) >> 27U);
            csd->readCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x7000000U) >> 24U);
            csd->writeCurrentVddMin = (uint8_t)((rawCsd[1U] & 0xE00000U) >> 20U);
            csd->writeCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x1C0000U) >> 18U);
            csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x38000U) >> 15U);

            /* Get card total block count and block size. */
            card->blockCount = ((csd->deviceSize + 1U) << (csd->deviceSizeMultiplier + 2U));
            card->blockSize = (1U << (csd->readBlockLength));
            if (card->blockSize != FSL_SDMMC_DEFAULT_BLOCK_SIZE)
            {
                card->blockCount = (card->blockCount * card->blockSize);
                card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
                card->blockCount = (card->blockCount / card->blockSize);
            }
            break;
        case 1:
            card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;

            csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3FU) << 16U);
            csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xFFFF0000U) >> 16U);
            if (csd->deviceSize >= 0xFFFFU)
            {
                card->flags |= kSD_SupportSdxcFlag;
            }

            card->blockCount = ((csd->deviceSize + 1U) * 1024U);
            break;
        default:
            break;
    }
    if ((uint8_t)((rawCsd[1U] & 0x4000U) >> 14U))
    {
        csd->flags |= kSD_CsdEraseBlockEnabledFlag;
    }
    csd->eraseSectorSize = (uint8_t)((rawCsd[1U] & 0x3F80U) >> 7U);
    csd->writeProtectGroupSize = (uint8_t)(rawCsd[1U] & 0x7FU);
    if ((uint8_t)(rawCsd[0U] & 0x80000000U))
    {
        csd->flags |= kSD_CsdWriteProtectGroupEnabledFlag;
    }
    csd->writeSpeedFactor = (uint8_t)((rawCsd[0U] & 0x1C000000U) >> 26U);
    csd->writeBlockLength = (uint8_t)((rawCsd[0U] & 0x3C00000U) >> 22U);
    if ((uint8_t)((rawCsd[0U] & 0x200000U) >> 21U))
    {
        csd->flags |= kSD_CsdWriteBlockPartialFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x8000U) >> 15U))
    {
        csd->flags |= kSD_CsdFileFormatGroupFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x4000U) >> 14U))
    {
        csd->flags |= kSD_CsdCopyFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x2000U) >> 13U))
    {
        csd->flags |= kSD_CsdPermanentWriteProtectFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x1000U) >> 12U))
    {
        csd->flags |= kSD_CsdTemporaryWriteProtectFlag;
    }
    csd->fileFormat = (uint8_t)((rawCsd[0U] & 0xC00U) >> 10U);
}

static status_t SD_SendCsd(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_SendCsd;
    command.argument = (card->relativeAddress << 16U);
    command.responseType = kCARD_ResponseTypeR2;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success == card->host.transfer(card->host.base, &content))
    {
        memcpy(card->rawCsd, command.response, sizeof(card->rawCsd));
        /* The response is from bit 127:8 in R2, corrisponding to command.response[3U]:command.response[0U][31U:8]. */
        SD_DecodeCsd(card, command.response);

        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static void SD_DecodeCid(sd_card_t *card, uint32_t *rawCid)
{
    assert(card);
    assert(rawCid);

    sd_cid_t *cid;

    cid = &(card->cid);
    cid->manufacturerID = (uint8_t)((rawCid[3U] & 0xFF000000U) >> 24U);
    cid->applicationID = (uint16_t)((rawCid[3U] & 0xFFFF00U) >> 8U);

    cid->productName[0U] = (uint8_t)((rawCid[3U] & 0xFFU));
    cid->productName[1U] = (uint8_t)((rawCid[2U] & 0xFF000000U) >> 24U);
    cid->productName[2U] = (uint8_t)((rawCid[2U] & 0xFF0000U) >> 16U);
    cid->productName[3U] = (uint8_t)((rawCid[2U] & 0xFF00U) >> 8U);
    cid->productName[4U] = (uint8_t)((rawCid[2U] & 0xFFU));

    cid->productVersion = (uint8_t)((rawCid[1U] & 0xFF000000U) >> 24U);

    cid->productSerialNumber = (uint32_t)((rawCid[1U] & 0xFFFFFFU) << 8U);
    cid->productSerialNumber |= (uint32_t)((rawCid[0U] & 0xFF000000U) >> 24U);

    cid->manufacturerData = (uint16_t)((rawCid[0U] & 0xFFF00U) >> 8U);
}

static status_t SD_AllSendCid(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    command.index = kSDMMC_AllSendCid;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR2;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success == card->host.transfer(card->host.base, &content))
    {
        memcpy(card->rawCid, command.response, sizeof(card->rawCid));
        SD_DecodeCid(card, command.response);

        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static status_t SD_ApplicationSendOperationCondition(sd_card_t *card, uint32_t argument)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    status_t error = kStatus_Fail;
    uint32_t i = FSL_SDMMC_MAX_VOLTAGE_RETRIES;

    command.index = kSD_ApplicationSendOperationCondition;
    command.argument = argument;
    command.responseType = kCARD_ResponseTypeR3;

    while (i--)
    {
        if (kStatus_Success != SD_SendApplicationCmd(card, 0U))
        {
            continue;
        }

        content.command = &command;
        content.data = NULL;
        if (kStatus_Success != card->host.transfer(card->host.base, &content))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        /* Wait until card exit busy state. */
        if (command.response[0U] & kSD_OcrPowerUpBusyFlag)
        {
            /* high capacity check */
            if (command.response[0U] & kSD_OcrCardCapacitySupportFlag)
            {
                card->flags |= kSD_SupportHighCapacityFlag;
            }
            /* 1.8V support */
            if (command.response[0U] & kSD_OcrSwitch18AcceptFlag)
            {
                card->flags |= kSD_SupportVoltage180v;
            }
            error = kStatus_Success;
            card->ocr = command.response[0U];
            break;
        }
        error = kStatus_Timeout;
    }

    return error;
}

static status_t SD_BL_ApplicationSendOperationCondition(sd_card_t *card, uint32_t argument)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    status_t error = kStatus_Fail;
    uint32_t i = FSL_SDMMC_MAX_VOLTAGE_RETRIES;

    command.index = kSD_ApplicationSendOperationCondition;
    command.argument = argument;
    command.responseType = kCARD_ResponseTypeR3;

    uint64_t timeoutTicks = microseconds_get_ticks() + microseconds_convert_to_ticks(SDMMC_WAIT_BUSY_TIMEOUT_US);

    while ((i--) && (microseconds_get_ticks() < timeoutTicks))
    {
        error = SD_SendApplicationCmd(card, 0U);
        if (kStatus_Success != error)
        {
            return error;
        }

        content.command = &command;
        content.data = NULL;
        if (kStatus_Success != card->host.transfer(card->host.base, &content))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        /* Wait until card exit busy state. */
        if (command.response[0U] & kSD_OcrPowerUpBusyFlag)
        {
            /* high capacity check */
            if (command.response[0U] & kSD_OcrCardCapacitySupportFlag)
            {
                card->flags |= kSD_SupportHighCapacityFlag;
            }
            /* 1.8V support */
            if (command.response[0U] & kSD_OcrSwitch18AcceptFlag)
            {
                card->flags |= kSD_SupportVoltage180v;
            }
            error = kStatus_Success;
            card->ocr = command.response[0U];
            break;
        }
        error = kStatus_Timeout;
    }

    return error;
}

static status_t SD_SendInterfaceCondition(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    uint32_t i = FSL_SDMMC_MAX_CMD_RETRIES;
    status_t error;

    command.index = kSD_SendInterfaceCondition;
    command.argument = 0x1AAU;
    command.responseType = kCARD_ResponseTypeR7;

    content.command = &command;
    content.data = NULL;
    do
    {
        if (kStatus_Success != card->host.transfer(card->host.base, &content))
        {
            error = kStatus_SDMMC_TransferFailed;
        }
        else
        {
            if ((command.response[0U] & 0xFFU) != 0xAAU)
            {
                error = kStatus_SDMMC_CardNotSupport;
            }
            else
            {
                error = kStatus_Success;
            }
        }
    } while (--i && (error != kStatus_Success));

    return error;
}

static status_t SD_BL_SendInterfaceCondition(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    uint32_t i = FSL_SDMMC_MAX_CMD_RETRIES;
    status_t error;

    command.index = kSD_SendInterfaceCondition;
    command.argument = 0x1AAU;
    command.responseType = kCARD_ResponseTypeR7;

    content.command = &command;
    content.data = NULL;
    do
    {
        if (kStatus_Success != card->host.transfer(card->host.base, &content))
        {
            error = kStatus_SDMMC_TransferFailed;
        }
        else
        {
            if ((command.response[0U] & 0x1FFU) != 0x1AAU)
            {
                error = kStatus_SDMMC_CardNotSupport;
            }
            else
            {
                error = kStatus_Success;
            }
        }
    } while (--i && (error != kStatus_Success) && (error != kStatus_SDMMC_CardNotSupport));

    return error;
}

static status_t SD_SelectBusTiming(sd_card_t *card)
{
    assert(card);

    status_t error = kStatus_SDMMC_SwitchBusTimingFailed;

    if (card->operationVoltage != kCARD_OperationVoltage180V)
    {
        /* Switch the card to high speed mode */
        if (card->host.capability.flags & kSDMMCHOST_SupportHighSpeed)
        {
            /* group 1, function 1 ->high speed mode*/
            error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR25HighSpeed);
            /* If the result isn't "switching to high speed mode(50MHZ) successfully or card doesn't support high speed
             * mode". Return failed status. */
            if (error == kStatus_Success)
            {
                card->currentTiming = kSD_TimingSDR25HighSpeedMode;
                card->busClock_Hz =
                    SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_50MHZ);
            }
            else if (error == kStatus_SDMMC_NotSupportYet)
            {
                /* if not support high speed, keep the card work at default mode */
                return kStatus_Success;
            }
        }
        else
        {
            /* if not support high speed, keep the card work at default mode */
            return kStatus_Success;
        }
    }
    /* card is in UHS_I mode */
    else if ((kSDMMCHOST_SupportSDR104 != SDMMCHOST_NOT_SUPPORT) ||
             (kSDMMCHOST_SupportSDR50 != SDMMCHOST_NOT_SUPPORT) || (kSDMMCHOST_SupportDDR50 != SDMMCHOST_NOT_SUPPORT))
    {
        switch (card->currentTiming)
        {
            /* if not select timing mode, sdmmc will handle it automatically*/
            case kSD_TimingSDR12DefaultMode:
            case kSD_TimingSDR104Mode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR104);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingSDR104Mode;
                    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz,
                                                                 SDMMCHOST_SUPPORT_SDR104_FREQ);
                    break;
                }
            case kSD_TimingDDR50Mode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionDDR50);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingDDR50Mode;
                    card->busClock_Hz =
                        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_100MHZ);
                    SDMMCHOST_ENABLE_DDR_MODE(card->host.base, true, 0U);
                }
                break;
            case kSD_TimingSDR50Mode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR50);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingSDR50Mode;
                    card->busClock_Hz =
                        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_100MHZ);
                }
                break;
            case kSD_TimingSDR25HighSpeedMode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR25HighSpeed);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingSDR25HighSpeedMode;
                    card->busClock_Hz =
                        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_50MHZ);
                }
                break;

            default:
                break;
        }
    }
    else
    {
    }

    /* SDR50 and SDR104 mode need tuning */
    if ((card->currentTiming == kSD_TimingSDR50Mode) || (card->currentTiming == kSD_TimingSDR104Mode))
    {
        /* config IO strength in IOMUX*/
        if (card->currentTiming == kSD_TimingSDR50Mode)
        {
            SDMMCHOST_CONFIG_SD_IO(CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_7);
        }
        else
        {
            SDMMCHOST_CONFIG_SD_IO(CARD_BUS_FREQ_200MHZ, CARD_BUS_STRENGTH_7);
        }
        /* execute tuning */
        if (SD_ExecuteTuning(card) != kStatus_Success)
        {
            return kStatus_SDMMC_TuningFail;
        }
    }
    else
    {
        /* set default IO strength to 4 to cover card adapter driver strength difference */
        SDMMCHOST_CONFIG_SD_IO(CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_4);
    }

    return error;
}

static status_t SD_BL_SelectBusTiming(sd_card_t *card)
{
    assert(card);

    status_t error = kStatus_SDMMC_SwitchBusTimingFailed;
    uint32_t funcTiming, freq;

    switch (card->userConfig.timing)
    {
        case kSD_TimingDDR50Mode:
            if (kSDMMCHOST_SupportDDR50 != SDMMCHOST_NOT_SUPPORT)
            {
                funcTiming = kSD_FunctionDDR50;
                freq = SD_CLOCK_100MHZ;
                break;
            }
            else
            {
                return kStatus_SDMMC_HostNotSupport;
            }
        case kSD_TimingSDR104Mode:
            if (kSDMMCHOST_SupportSDR104 != SDMMCHOST_NOT_SUPPORT)
            {
                funcTiming = kSD_FunctionSDR104;
                freq = SDMMCHOST_SUPPORT_SDR104_FREQ;
                break;
            }
            else
            {
                return kStatus_SDMMC_HostNotSupport;
            }
        case kSD_TimingSDR50Mode:
            if (kSDMMCHOST_SupportSDR50 != SDMMCHOST_NOT_SUPPORT)
            {
                funcTiming = kSD_FunctionSDR50;
                freq = SD_CLOCK_100MHZ;
                break;
            }
            else
            {
                return kStatus_SDMMC_HostNotSupport;
            }
        case kSD_TimingSDR25HighSpeedMode:
            if (kSDMMCHOST_SupportHighSpeed != SDMMCHOST_NOT_SUPPORT)
            {
                funcTiming = kSD_FunctionSDR25HighSpeed;
                freq = SD_CLOCK_50MHZ;
                break;
            }
            else
            {
                return kStatus_SDMMC_HostNotSupport;
            }
        case kSD_TimingSDR12DefaultMode:
        default:
            return kStatus_Success;
    }

    error = SD_SelectFunction(card, kSD_GroupTimingMode, funcTiming);
    if (error == kStatus_Success)
    {
        card->currentTiming = card->userConfig.timing;
        card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, freq);
    }
    else
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    /* SDR50 and SDR104 mode need tuning */
    if ((card->currentTiming == kSD_TimingSDR50Mode) || (card->currentTiming == kSD_TimingSDR104Mode))
    {
        /* config IO strength in IOMUX*/
        if (card->currentTiming == kSD_TimingSDR50Mode)
        {
            SDMMCHOST_UPDATE_SD_IO(card->host.base, card->userConfig.busWidth, CARD_BUS_FREQ_100MHZ1,
                                   CARD_BUS_STRENGTH_7);
        }
        else
        {
            SDMMCHOST_UPDATE_SD_IO(card->host.base, card->userConfig.busWidth, CARD_BUS_FREQ_200MHZ,
                                   CARD_BUS_STRENGTH_7);
        }
        /* execute tuning */
        if (SD_ExecuteTuning(card) != kStatus_Success)
        {
            return kStatus_SDMMC_TuningFail;
        }
    }
    else
    {
        SDMMCHOST_UPDATE_SD_IO(card->host.base, card->userConfig.busWidth, CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_4);
    }

    return error;
}

static status_t SD_SetDriverStrength(sd_card_t *card, sd_driver_strength_t driverStrength)
{
    assert(card);

    status_t error;
    uint32_t strength = driverStrength;

    error = SD_SelectFunction(card, kSD_GroupDriverStrength, strength);

    return error;
}

static status_t SD_SetMaxCurrent(sd_card_t *card, sd_max_current_t maxCurrent)
{
    assert(card);

    status_t error;
    uint32_t current = maxCurrent;

    error = SD_SelectFunction(card, kSD_GroupCurrentLimit, current);

    return error;
}

static status_t SD_Read(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);
    assert(blockSize);
    assert(blockSize == FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    SDMMCHOST_DATA data = { 0 };
    status_t error;

    if (((card->flags & kSD_SupportHighCapacityFlag) && (blockSize != 512U)) || (blockSize > card->blockSize) ||
        (blockSize > card->host.capability.maxBlockLength) || (blockSize % 4))
    {
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer. */
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.rxData = (uint32_t *)buffer;

    command.index = kSDMMC_ReadMultipleBlock;
    if (data.blockCount == 1U)
    {
        command.index = kSDMMC_ReadSingleBlock;
    }
    command.argument = startBlock;
    if (!(card->flags & kSD_SupportHighCapacityFlag))
    {
        command.argument *= data.blockSize;
    }
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = kSDMMC_R1ErrorAllFlag;

    content.command = &command;
    content.data = &data;

    error = SD_Transfer(card, &content, 1U);
    if (kStatus_Success != error)
    {
        return error;
    }

    /* Send STOP_TRANSMISSION command in multiple block transmission and host's AUTO_COMMAND12 isn't enabled. */
    if ((data.blockCount > 1U) && (!(data.enableAutoCommand12)))
    {
        if (kStatus_Success != SD_StopTransmission(card))
        {
            return kStatus_SDMMC_StopTransmissionFailed;
        }
    }

    return kStatus_Success;
}

static status_t SD_Write(
    sd_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);
    assert(blockSize);
    assert(blockSize == FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };
    SDMMCHOST_DATA data = { 0 };
    status_t error;

    if (((card->flags & kSD_SupportHighCapacityFlag) && (blockSize != 512U)) || (blockSize > card->blockSize) ||
        (blockSize > card->host.capability.maxBlockLength) || (blockSize % 4U))
    {
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Wait for the card's buffer to be not full to write to improve the write performance. */
    while ((GET_SDMMCHOST_STATUS(card->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY)
    {
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer.*/
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.txData = (const uint32_t *)buffer;

    command.index = kSDMMC_WriteMultipleBlock;
    if (data.blockCount == 1U)
    {
        command.index = kSDMMC_WriteSingleBlock;
    }
    command.argument = startBlock;
    if (!(card->flags & kSD_SupportHighCapacityFlag))
    {
        command.argument *= data.blockSize;
    }
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = kSDMMC_R1ErrorAllFlag;

    content.command = &command;
    content.data = &data;

    error = SD_Transfer(card, &content, 1U);
    if (kStatus_Success != error)
    {
        return error;
    }

    /* Send STOP_TRANSMISSION command in multiple block transmission and host's AUTO_COMMAND12 isn't enabled. */
    if ((data.blockCount > 1U) && (!(data.enableAutoCommand12)))
    {
        if (kStatus_Success != SD_StopTransmission(card))
        {
            return kStatus_SDMMC_StopTransmissionFailed;
        }
    }

    return kStatus_Success;
}

static status_t SD_Erase(sd_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);

    uint32_t eraseBlockStart;
    uint32_t eraseBlockEnd;
    SDMMCHOST_TRANSFER content = { 0 };
    SDMMCHOST_COMMAND command = { 0 };

    /* Wait for the card's buffer to be not full to write to improve the write performance. */
    while ((GET_SDMMCHOST_STATUS(card->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY)
    {
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer.*/
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    eraseBlockStart = startBlock;
    eraseBlockEnd = eraseBlockStart + blockCount - 1U;
    if (!(card->flags & kSD_SupportHighCapacityFlag))
    {
        eraseBlockStart = eraseBlockStart * FSL_SDMMC_DEFAULT_BLOCK_SIZE;
        eraseBlockEnd = eraseBlockEnd * FSL_SDMMC_DEFAULT_BLOCK_SIZE;
    }

    /* Send ERASE_WRITE_BLOCK_START command to set the start block number to erase. */
    command.index = kSD_EraseWriteBlockStart;
    command.argument = eraseBlockStart;
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = kSDMMC_R1ErrorAllFlag;

    content.command = &command;
    content.data = NULL;

    if (kStatus_Success != SD_Transfer(card, &content, 1U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Send ERASE_WRITE_BLOCK_END command to set the end block number to erase. */
    command.index = kSD_EraseWriteBlockEnd;
    command.argument = eraseBlockEnd;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != SD_Transfer(card, &content, 0U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Send ERASE command to start erase process. */
    command.index = kSDMMC_Erase;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1b;
    command.responseErrorFlags = kSDMMC_R1ErrorAllFlag;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != SD_Transfer(card, &content, 0U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

bool SD_CheckReadOnly(sd_card_t *card)
{
    assert(card);

    return ((card->csd.flags & kSD_CsdPermanentWriteProtectFlag) ||
            (card->csd.flags & kSD_CsdTemporaryWriteProtectFlag));
}

status_t SD_ReadBlocks(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);

    uint32_t blockCountOneTime;
    uint32_t blockLeft;
    uint32_t blockDone;
    uint8_t *nextBuffer;
    status_t error;

    if ((blockCount + startBlock) > card->blockCount)
    {
        return kStatus_InvalidArgument;
    }

    blockLeft = blockCount;
    blockDone = 0U;
    while (blockLeft)
    {
        if (blockLeft > card->host.capability.maxBlockCount)
        {
            blockLeft = (blockLeft - card->host.capability.maxBlockCount);
            blockCountOneTime = card->host.capability.maxBlockCount;
        }
        else
        {
            blockCountOneTime = blockLeft;
            blockLeft = 0U;
        }

        nextBuffer = (buffer + blockDone * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        error = SD_Read(card, nextBuffer, (startBlock + blockDone), FSL_SDMMC_DEFAULT_BLOCK_SIZE, blockCountOneTime);
        if (error != kStatus_Success)
        {
            return error;
        }

        blockDone += blockCountOneTime;
    }

    return kStatus_Success;
}

status_t SD_WriteBlocks(sd_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);

    uint32_t blockCountOneTime; /* The block count can be wrote in one time sending WRITE_BLOCKS command. */
    uint32_t blockLeft;         /* Left block count to be wrote. */
    uint32_t blockDone = 0U;    /* The block count has been wrote. */
    const uint8_t *nextBuffer;
    status_t error;

    if ((blockCount + startBlock) > card->blockCount)
    {
        return kStatus_InvalidArgument;
    }

    blockLeft = blockCount;
    while (blockLeft)
    {
        if (blockLeft > card->host.capability.maxBlockCount)
        {
            blockLeft = (blockLeft - card->host.capability.maxBlockCount);
            blockCountOneTime = card->host.capability.maxBlockCount;
        }
        else
        {
            blockCountOneTime = blockLeft;
            blockLeft = 0U;
        }

        nextBuffer = (buffer + blockDone * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        error = SD_Write(card, nextBuffer, (startBlock + blockDone), FSL_SDMMC_DEFAULT_BLOCK_SIZE, blockCountOneTime);
        if (error != kStatus_Success)
        {
            return error;
        }

        blockDone += blockCountOneTime;
    }

    return kStatus_Success;
}

status_t SD_EraseBlocks(sd_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);

    uint32_t blockCountOneTime; /* The block count can be erased in one time sending ERASE_BLOCKS command. */
    uint32_t blockDone = 0U;    /* The block count has been erased. */
    uint32_t blockLeft;         /* Left block count to be erase. */
    status_t error;

    if ((blockCount + startBlock) > card->blockCount)
    {
        return kStatus_InvalidArgument;
    }

    blockLeft = blockCount;
    while (blockLeft)
    {
        if (blockLeft > (card->csd.eraseSectorSize + 1U))
        {
            blockCountOneTime = card->csd.eraseSectorSize + 1U;
            blockLeft = blockLeft - blockCountOneTime;
        }
        else
        {
            blockCountOneTime = blockLeft;
            blockLeft = 0U;
        }

        error = SD_Erase(card, (startBlock + blockDone), blockCountOneTime);
        if (error != kStatus_Success)
        {
            return error;
        }

        blockDone += blockCountOneTime;
    }

    return kStatus_Success;
}

status_t SD_CardInit(sd_card_t *card)
{
    assert(card);

    uint32_t applicationCommand41Argument = 0U;

    if (!card->isHostReady)
    {
        return kStatus_SDMMC_HostNotReady;
    }
    /* reset variables */
    card->flags = 0U;
    /* set DATA bus width */
    SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH1BIT);
    /*set card freq to 400KHZ*/
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SDMMC_CLOCK_400KHZ);
    /* send card active */
    SDMMCHOST_SEND_CARD_ACTIVE(card->host.base, 100U);
    /* Get host capability. */
    GET_SDMMCHOST_CAPABILITY(card->host.base, &(card->host.capability));

    /* card go idle */
    if (kStatus_Success != SD_GoIdle(card))
    {
        return kStatus_SDMMC_GoIdleFailed;
    }

    if (kSDMMCHOST_SupportV330 != SDMMCHOST_NOT_SUPPORT)
    {
        applicationCommand41Argument |= (kSD_OcrVdd32_33Flag | kSD_OcrVdd33_34Flag);
        card->operationVoltage = kCARD_OperationVoltage330V;
    }
    else if (kSDMMCHOST_SupportV300 != SDMMCHOST_NOT_SUPPORT)
    {
        applicationCommand41Argument |= kSD_OcrVdd29_30Flag;
        card->operationVoltage = kCARD_OperationVoltage330V;
    }

    /* allow user select the work voltage, if not select, sdmmc will handle it automatically */
    if (kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT)
    {
        applicationCommand41Argument |= kSD_OcrSwitch18RequestFlag;
    }

    /* Check card's supported interface condition. */
    if (kStatus_Success == SD_SendInterfaceCondition(card))
    {
        /* SDHC or SDXC card */
        applicationCommand41Argument |= kSD_OcrHostCapacitySupportFlag;
        card->flags |= kSD_SupportSdhcFlag;
    }
    else
    {
        /* SDSC card */
        if (kStatus_Success != SD_GoIdle(card))
        {
            return kStatus_SDMMC_GoIdleFailed;
        }
    }
    /* Set card interface condition according to SDHC capability and card's supported interface condition. */
    if (kStatus_Success != SD_ApplicationSendOperationCondition(card, applicationCommand41Argument))
    {
        return kStatus_SDMMC_HandShakeOperationConditionFailed;
    }

    /* check if card support 1.8V */
    if ((card->flags & kSD_SupportVoltage180v))
    {
        if (kStatus_Success != SD_SwitchVoltage(card))
        {
            return kStatus_SDMMC_InvalidVoltage;
        }
        card->operationVoltage = kCARD_OperationVoltage180V;
    }

    /* Initialize card if the card is SD card. */
    if (kStatus_Success != SD_AllSendCid(card))
    {
        return kStatus_SDMMC_AllSendCidFailed;
    }
    if (kStatus_Success != SD_SendRca(card))
    {
        return kStatus_SDMMC_SendRelativeAddressFailed;
    }
    if (kStatus_Success != SD_SendCsd(card))
    {
        return kStatus_SDMMC_SendCsdFailed;
    }
    if (kStatus_Success != SD_SelectCard(card, true))
    {
        return kStatus_SDMMC_SelectCardFailed;
    }

    if (kStatus_Success != SD_SendScr(card))
    {
        return kStatus_SDMMC_SendScrFailed;
    }

    /* Set to max frequency in non-high speed mode. */
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_25MHZ);

    /* Set to 4-bit data bus mode. */
    if (((card->host.capability.flags) & kSDMMCHOST_Support4BitBusWidth) && (card->flags & kSD_Support4BitWidthFlag))
    {
        if (kStatus_Success != SD_SetDataBusWidth(card, kSD_DataBusWidth4Bit))
        {
            return kStatus_SDMMC_SetDataBusWidthFailed;
        }
        SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH4BIT);
    }

    /* set sd card driver strength */
    SD_SetDriverStrength(card, card->driverStrength);
    /* set sd card current limit */
    SD_SetMaxCurrent(card, card->maxCurrent);

    /* set block size */
    if (SD_SetBlockSize(card, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
    {
        return kStatus_SDMMC_SetCardBlockSizeFailed;
    }

    /* select bus timing */
    if (kStatus_Success != SD_SelectBusTiming(card))
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    return kStatus_Success;
}

status_t SD_BL_CardInit(sd_card_t *card)
{
    assert(card);

    uint32_t applicationCommand41Argument = 0U;
    status_t error = kStatus_Success;
    bool isUHSImode = false;

    if (!card->isHostReady)
    {
        return kStatus_SDMMC_HostNotReady;
    }

    /* init the pin mux here. */
    SDMMCHOST_CONFIG_SD_MUX(card->host.base, card->userConfig.busWidth);

    isUHSImode = (card->userConfig.timing == kSD_TimingSDR50Mode) ||
                 (card->userConfig.timing == kSD_TimingSDR104Mode) || (card->userConfig.timing == kSD_TimingDDR50Mode);
    /* execute the power cycle here */
    if ((card->userConfig.enablePowerCycle) || (isUHSImode))
    {
        SDMMCHOST_INIT_SD_RESET(card->host.base);

        /* card power off */
        SDMMCHOST_SET_SD_RESET(card->host.base, card->userConfig.powerPolarity);
        /* Delay some time to make card stable. */
        microseconds_delay(card->userConfig.powerDownDelay_US);
        /* card power on */
        SDMMCHOST_SET_SD_RESET(card->host.base, !card->userConfig.powerPolarity);
        /* Delay some time to make card stable. */
        microseconds_delay(card->userConfig.powerUpDelay_US);
    }

    if (isUHSImode)
    {
        /* UHSI mode only supports 4bit. Update to 4bit mode even config 1bit. */
        card->userConfig.busWidth = kSD_DataBusWidth4Bit;
        /* init vselect pin */
        SDMMCHOST_INIT_SD_VSEL(card->host.base);
    }

    /* reset variables */
    card->flags = 0U;
    /* set DATA bus width */
    SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH1BIT);
    /*set card freq to 300KHZ*/
    /*
     * Note: according to spec, the maximum frequency at identification state is 400KHz. But considering clock jitter,
     * and some devices might not tolerant the frequency is higher than 400KHz, 300KHz is more compatible.
     */
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SDMMC_CLOCK_300KHZ);
    /* send card active */
    SDMMCHOST_SEND_CARD_ACTIVE(card->host.base, 10000U);
    /* Get host capability. */
    GET_SDMMCHOST_CAPABILITY(card->host.base, &(card->host.capability));

    /* card go idle */
    if (kStatus_Success != SD_GoIdle(card))
    {
        return kStatus_SDMMC_GoIdleFailed;
    }

    // if (kHOST_SupportV330 != HOST_NOT_SUPPORT)
    //{
    //    applicationCommand41Argument |= (kSD_OcrVdd32_33Flag | kSD_OcrVdd33_34Flag);
    //    card->operationVoltage = kCARD_OperationVoltage330V;
    //}
    // if (kHOST_SupportV300 != HOST_NOT_SUPPORT)
    //{
    //    applicationCommand41Argument |= kSD_OcrVdd29_30Flag;
    //    card->operationVoltage = kCARD_OperationVoltage330V;
    //}
    // For bootloader, check all supplied voltage level.
    applicationCommand41Argument |=
        (kSD_OcrVdd27_28Flag | kSD_OcrVdd28_29Flag | kSD_OcrVdd29_30Flag | kSD_OcrVdd30_31Flag | kSD_OcrVdd31_32Flag |
         kSD_OcrVdd32_33Flag | kSD_OcrVdd33_34Flag | kSD_OcrVdd34_35Flag | kSD_OcrVdd35_36Flag);
    card->operationVoltage = kCARD_OperationVoltage330V;

    /* allow user select the work voltage, if not select, sdmmc will handle it automatically */
    if (kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT)
    {
        if (isUHSImode)
        {
            applicationCommand41Argument |= kSD_OcrSwitch18RequestFlag;
        }
    }
    else
    {
        if (isUHSImode)
        {
            return kStatus_SDMMC_HostNotSupport;
        }
    }

    /* Check card's supported interface condition. */
    error = SD_BL_SendInterfaceCondition(card);
    if (kStatus_Success == error)
    {
        /* Ver2.00 or later SD card */
        applicationCommand41Argument |= kSD_OcrHostCapacitySupportFlag;
        card->flags |= kSD_SupportSdhcFlag;
    }
    else if (kStatus_SDMMC_CardNotSupport == error)
    {
        return error;
    }
    else /* No response */
    {
        /* Ver2.00 or later SD Memory Card(voltage mismatch),
         * or Ver1.X SD card, or not SD card
         */
        if (kStatus_Success != SD_GoIdle(card))
        {
            return kStatus_SDMMC_GoIdleFailed;
        }
    }
    /* Set card interface condition according to SDHC capability and card's supported interface condition. */
    if (kStatus_Success != SD_BL_ApplicationSendOperationCondition(card, applicationCommand41Argument))
    {
        return kStatus_SDMMC_HandShakeOperationConditionFailed;
    }

    /* check if card support 1.8V */
    if (card->flags & kSD_SupportVoltage180v)
    {
        if (kStatus_Success != SD_SwitchVoltage(card))
        {
            return kStatus_SDMMC_InvalidVoltage;
        }
        card->operationVoltage = kCARD_OperationVoltage180V;
    }
    else if (applicationCommand41Argument & kSD_OcrSwitch18RequestFlag)
    {
        // 1V8 is request, but not supported by SD card.
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Initialize card if the card is SD card. */
    if (kStatus_Success != SD_AllSendCid(card))
    {
        return kStatus_SDMMC_AllSendCidFailed;
    }
    if (kStatus_Success != SD_SendRca(card))
    {
        return kStatus_SDMMC_SendRelativeAddressFailed;
    }
    if (kStatus_Success != SD_SendCsd(card))
    {
        return kStatus_SDMMC_SendCsdFailed;
    }
    if (kStatus_Success != SD_SelectCard(card, true))
    {
        return kStatus_SDMMC_SelectCardFailed;
    }

    /* Set to max frequency in non-high speed mode. */
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_25MHZ);

    /* Set 4 bit data bus mode. */
    if ((card->userConfig.busWidth == kSD_DataBusWidth4Bit) || isUHSImode)
    {
        if (kSDMMCHOST_Support4BitBusWidth != SDMMCHOST_NOT_SUPPORT)
        {
            /* Note: No need to check if 4bit is supported or not.
             * SPEC writes: "SD Memory Card shall support at least the two bus modes 1-bit or 4-bit width."
             */
            // if (card->flags & kSD_Support4BitWidthFlag)
            //{
            if (kStatus_Success != SD_SetDataBusWidth(card, kSD_DataBusWidth4Bit))
            {
                return kStatus_SDMMC_SetDataBusWidthFailed;
            }
            SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH4BIT);
            //}
            // else
            //{
            //    return kStatus_SDMMC_CardNotSupport;
            //}
        }
        else
        {
            return kStatus_SDMMC_HostNotSupport;
        }
    }

    if (kStatus_Success != SD_SendScr(card))
    {
        return kStatus_SDMMC_SendScrFailed;
    }

    /* Not necessary for bootloader. */
    /* set sd card driver strength */
    // SD_SetDriverStrength(card, card->driverStrength);
    /* set sd card current limit */
    // SD_SetMaxCurrent(card, card->maxCurrent);

    /* set block size */
    if (SD_SetBlockSize(card, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
    {
        return kStatus_SDMMC_SetCardBlockSizeFailed;
    }

    /* select bus timing */
    error = SD_BL_SelectBusTiming(card);
    if (error != kStatus_Success)
    {
        return error;
    }

    return kStatus_Success;
}

void SD_CardDeinit(sd_card_t *card)
{
    assert(card);

    SD_SelectCard(card, false);
}

status_t SD_HostInit(sd_card_t *card)
{
    assert(card);

    if ((!card->isHostReady) && SDMMCHOST_Init(&(card->host), (void *)(card->usrParam.cd)) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* set the host status flag, after the card re-plug in, don't need init host again */
    card->isHostReady = true;

    return kStatus_Success;
}

void SD_HostDeinit(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_Deinit(&(card->host));
    /* should re-init host */
    card->isHostReady = false;
}

void SD_HostReset(SDMMCHOST_CONFIG *host)
{
    SDMMCHOST_Reset(host->base);
}

void SD_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    SDMMCHOST_PowerOnCard(base, pwr);
}

void SD_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    SDMMCHOST_PowerOffCard(base, pwr);
}

status_t SD_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase, const sdmmchost_detect_card_t *cd, bool waitCardStatus)
{
    return SDMMCHOST_WaitCardDetectStatus(hostBase, cd, waitCardStatus);
}

bool SD_IsCardPresent(sd_card_t *card)
{
    return SDMMCHOST_IsCardPresent();
}

status_t SD_Init(sd_card_t *card)
{
    assert(card);

    if (!card->isHostReady)
    {
        if (SD_HostInit(card) != kStatus_Success)
        {
            return kStatus_SDMMC_HostNotReady;
        }
    }
    else
    {
        SD_HostReset(&(card->host));
    }
    SD_PowerOffCard(card->host.base, card->usrParam.pwr);

    if (SD_WaitCardDetectStatus(card->host.base, card->usrParam.cd, true) != kStatus_Success)
    {
        return kStatus_SDMMC_CardDetectFailed;
    }
    SD_PowerOnCard(card->host.base, card->usrParam.pwr);

    return SD_CardInit(card);
}

status_t SD_BL_Init(sd_card_t *card)
{
    assert(card);

    SD_HostReset(&(card->host));
    if (SD_HostInit(card) != kStatus_Success)
    {
        return kStatus_SDMMC_HostNotReady;
    }

    return SD_BL_CardInit(card);
}

void SD_Deinit(sd_card_t *card)
{
    /* card deinitialize */
    SD_CardDeinit(card);
    /* host deinitialize */
    SD_HostDeinit(card);
}
