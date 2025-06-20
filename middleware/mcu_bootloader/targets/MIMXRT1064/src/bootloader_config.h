/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __BOOTLOADER_CONFIG_H__
#define __BOOTLOADER_CONFIG_H__

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//
// Bootloader configuration options
//

//! @name Peripheral configuration macros
//@{

#define BL_FEATURE_ROM_UART_PORT (1)

// UART ports
#if !defined(BL_CONFIG_LPUART_1)
#define BL_CONFIG_LPUART_1 (BL_FEATURE_ROM_UART_PORT)
#define BL_FEATURE_UART_RX_PULLUP (1)
#endif

#define BL_CONFIG_LPUART (BL_CONFIG_LPUART_1)

#define BL_FEATURE_UID_IN_FUSE (1)

#define BL_FEATURE_GEN_KEYBLOB (1)
#define BL_FEATURE_KEYBLOB_BK_SIZE (16)

// USB HS port
#if !defined(BL_CONFIG_HS_USB_HID)
#define BL_CONFIG_HS_USB_HID (1U) // i.MX RT Series only supoort HS USB
#endif
//@}

#if !defined(BL_TARGET_FLASH) && !defined(BL_TARGET_RAM)
#define BL_TARGET_FLASH (0)
#endif

// Internal Flash features
#define BL_FEATURE_HAS_NO_INTERNAL_FLASH (1)

#if !BL_FEATURE_HAS_NO_INTERNAL_FLASH
#if defined(BL_TARGET_RAM)
#define BL_FEATURE_FLASH_SECURITY (0)
#else
#define BL_FEATURE_FLASH_SECURITY (1)
#endif
#define BL_FEATURE_ERASEALL_UNSECURE (0)
#define BL_FEATURE_FLASH_VERIFY_DISABLE (0)
#endif // !BL_FEATURE_HAS_NO_INTERNAL_FLASH

#define BL_FEATURE_HAS_NO_READ_SOURCE (1)

#define BL_FEATURE_MIN_PROFILE (0)

#if !defined(BL_TARGET_RAM)
#define BL_FEATURE_CRC_CHECK (1)
#endif

#define BL_FEATURE_UART_AUTOBAUD_IRQ (1)

#define BL_FEATURE_FLASH_CHECK_CUMULATIVE_WRITE (1)

#define BL_FEATURE_FLEXSPI_NOR_MODULE (1)
#define BL_FEATURE_HAS_FLEXSPI_NOR_ROMAPI (0)

#if BL_FEATURE_FLEXSPI_NOR_MODULE
#if defined(BL_FEATURE_FLEXSPI_NOR_SIP)
#define BL_FEATURE_FLEXSPI_NOR_MODULE_PERIPHERAL_INSTANCE (1)
#define BL_FLEXSPI_AMBA_BASE (0x70000000u)
#else
#define BL_FEATURE_FLEXSPI_NOR_MODULE_PERIPHERAL_INSTANCE (get_flexspinor_instance())
#define BL_FLEXSPI_AMBA_BASE (get_flexspinor_amba_base())
#endif
#define BL_FEATURE_FLEXSPI_NOR_MODULE_ERASE_VERIFY (1)
#define BL_FEATURE_FLEXSPI_ENCRYPT_PROGRAMMING (1)
#define BL_PROT_REGION_BLOCK_OFFSET(i) (0x400u * (i + 1))

#endif // BL_FEATURE_FLEXSPI_NOR_MODULE

#define BL_FEATURE_SEMC_NOR_MODULE (1)

// Memory expansion features
#define BL_FEATURE_EXPAND_MEMORY (1)

#define BL_FEATURE_SPINAND_MODULE (1)

#if BL_FEATURE_SPINAND_MODULE
#if (!BL_FEATURE_EXPAND_MEMORY) && (BL_FEATURE_SPINAND_MODULE)
#error "BL_FEATURE_EXPAND_MEMORY" must be enabled to enable the SPI NAND feature.
#endif
#define BL_FEATURE_SPINAND_MODULE_ERASE_VERIFY (1)
#define BL_FEATURE_SPINAND_MODULE_PERIPHERAL_FLEXSPI (1)
#define BL_FEATURE_SPINAND_MODULE_PERIPHERAL_INSTANCE (0)
#endif // BL_FEATURE_SPINAND_MODULE

#define BL_FEATURE_MMC_MODULE (1)
#if BL_FEATURE_MMC_MODULE
#define BL_FEATURE_MMC_MODULE_ERASE_VERIFY (1)
#define BL_FEATURE_MMC_MODULE_PERIPHERAL_INSTANCE (2)
#define BL_FEATURE_MMC_MODULE_ENABLE_PERMANENT_CONFIG (0)
#endif // BL_FEATURE_MMC_MODULE

#define BL_FEATURE_SD_MODULE (1)
#if BL_FEATURE_SD_MODULE
#define BL_FEATURE_SD_MODULE_ERASE_VERIFY (1)
#define BL_FEATURE_SD_MODULE_PERIPHERAL_INSTANCE (1)
#endif // BL_FEATURE_SD_MODULE

#define BL_FEATURE_SEMC_NAND_MODULE (1)

#if BL_FEATURE_FLEXSPI_NOR_MODULE || BL_FEATURE_SPINAND_MODULE || BL_FEATURE_EMMC_MODULE || BL_FEATURE_SD_MODULE || \
    BL_FEATURE_SEMC_NAND_MODULE
#if !BL_FEATURE_EXPAND_MEMORY
#if defined(BL_FEATURE_EXPAND_MEMORY)
#undef BL_FEATURE_EXPAND_MEMORY
#endif
#define BL_FEATURE_EXPAND_MEMORY (1)
#warning "BL_FEATURE_EXPAND_MEMORY" is enabled automatically to support none-XIP memory devices.
#endif // #if !BL_FEATURE_EXPAND_MEMORY
#define BL_FEATURE_EXTERNAL_MEMORY_PROPERTY (1)
#endif

#define BL_FEATURE_SPI_NOR_EEPROM_MODULE (1)
#if BL_FEATURE_SPI_NOR_EEPROM_MODULE
#define BL_FEATURE_SPI_NOR_EEPROM_MODULE_ERASE_VERIFY (1)
#endif // BL_FEATURE_SPI_NOR_EEPROM_MODULE

#define BL_FEATURE_OCOTP_MODULE (FSL_FEATURE_SOC_OCOTP_COUNT)

#define BL_FEATURE_EXPAND_PACKET_SIZE (1)
#define BL_EXPANDED_FRAMING_PACKET_SIZE (512)
// Make sure that BL_EXPANDED_USB_HID_PACKET_SIZE < 1018
#define BL_EXPANDED_USB_HID_PACKET_SIZE (1012)

// Bootloader peripheral detection default timeout in milliseconds
// After coming out of reset the bootloader will spin in a peripheral detection
// loop for this amount of time. A zero value means no time out.
#if DEBUG
#define BL_DEFAULT_PERIPHERAL_DETECT_TIMEOUT 0
#else
#define BL_DEFAULT_PERIPHERAL_DETECT_TIMEOUT 5000
#endif // DEBUG

// The bootloader will check this address for the application vector table upon startup.
#if !defined(BL_APP_VECTOR_TABLE_ADDRESS)
#define BL_APP_VECTOR_TABLE_ADDRESS 0xa000
#endif

////////////////////////////////////////////////////////////////////////////////
// Not Support Commands
////////////////////////////////////////////////////////////////////////////////
#define BL_FETAURE_RECV_SB (0)

#endif // __BOOTLOADER_CONFIG_H__
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
