/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP. Not a Contribution
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "bootutil/bootutil_public.h"
#include "bootutil/boot_hooks.h"
#include "bootutil_priv.h"
#include "bootutil/bootutil_log.h"

#include "fsl_debug_console.h"

#include "sblconfig.h"
#include "flash_map.h"
#include "flash_partitioning.h"

#ifdef CONFIG_ENCRYPT_XIP_EXT_OVERWRITE_ONLY
#include "encrypted_xip.h"
#endif

#ifdef NDEBUG
#undef assert
#define assert(x) ((void)(x))
#endif

int boot_read_image_header_hook(int img_index, int slot, struct image_header *img_head)
{
    return BOOT_HOOK_REGULAR;
}

fih_int boot_image_check_hook(int img_index, int slot)
{
    return BOOT_HOOK_REGULAR;
}

int boot_perform_update_hook(int img_index, struct image_header *img_head,
                             const struct flash_area *area)
{
    return BOOT_HOOK_REGULAR;
}

int boot_copy_region_pre_hook(int img_index, const struct flash_area *area, size_t size)
{
#ifdef CONFIG_ENCRYPT_XIP_EXT_OVERWRITE_ONLY
    status_t status;
    const uint32_t enc_region_start = area->fa_off + BOOT_FLASH_BASE;
       
    status = encrypted_xip_cfg_write(boot_flash_meta_map, enc_region_start, size);
    if (status != kStatus_Success)
        return -1;
    
    status = encrypted_xip_cfg_initEncryption(boot_flash_meta_map);
    if (status != kStatus_Success)
        return -1;
#endif
    return 0;
}

int boot_copy_region_post_hook(int img_index, const struct flash_area *area, size_t size)
{
#ifdef CONFIG_ENCRYPT_XIP_EXT_OVERWRITE_ONLY
    status_t status;
    
    status = encrypted_xip_cfg_confirm(boot_flash_meta_map, 0);
    if (status != kStatus_Success)
      return -1;
#endif
    return 0;
}

int boot_read_swap_state_primary_slot_hook(int image_index, struct boot_swap_state *state)
{
    return BOOT_HOOK_REGULAR;
}