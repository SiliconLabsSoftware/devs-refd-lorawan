/***************************************************************************//**
 * @file hal_bootloader.h
 * @brief Bootloader HAL Header File
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SMTC_HAL_BOOTLOADER_H
#define SMTC_HAL_BOOTLOADER_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "smtc_modem_hal.h"

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define BOOTLOADER_STORAGE_SLOT 0

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Get information about the bootloader and the storage slot used for FUOTA
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t hal_bootloader_get_slot_info(void);

/**
 * @brief Verify the application stored in the bootloader storage slot
 * @return true if the application is verified, false otherwise
 */
bool hal_bootloader_verify_application(void);

/**
 * @brief Erase the bootloader storage slot if it is not empty
 */
void hal_bootloader_erase_slot_if_needed(void);

/**
 * @brief Set the application stored in the bootloader storage slot to be booted and reboot the device
 * @param slotId The storage slot ID
 * @param offset The offset in the storage slot to start writing
 * @param buffer The data buffer to write
 * @param size The size of the data to write
 */
void hal_bootloader_read_modify_write(uint32_t slotId, uint32_t offset, const uint8_t* buffer, uint32_t size);

/**
 * @brief Set the application stored in the bootloader storage slot to be booted and reboot the device
 */
void hal_bootloader_setImage_and_reboot(void);

#endif /* SMTC_HAL_BOOTLOADER_H */
