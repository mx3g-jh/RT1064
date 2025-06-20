/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <osa.h>

#if defined(IW610)

#if defined(CONFIG_BT_IND_DNLD)

const uint8_t fw_cpu2[] = {
#include <uartspi_iw610_bt.bin.se.inc>
};

const unsigned char *bt_fw_bin   = (const unsigned char *)(void *)&fw_cpu2[0];
const unsigned int bt_fw_bin_len = sizeof(fw_cpu2);

#endif

#endif
