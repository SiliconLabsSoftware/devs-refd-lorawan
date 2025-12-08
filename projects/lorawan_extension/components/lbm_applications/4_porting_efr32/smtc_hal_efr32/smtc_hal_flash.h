/***************************************************************************//**
 * @file hal_crashlog.h
 * @brief Crashlog HAL Header File
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

#ifndef SMTC_HAL_FLASH_H
#define SMTC_HAL_FLASH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "stdint.h"
#include "stdbool.h"
#include "smtc_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#ifndef SUCCESS
#define SUCCESS 1
#endif

#ifndef FAIL
#define FAIL 0
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Init MSC module
 */
void hal_flash_init(void);

/**
 * @brief Get the flash page size
 * @return Flash page size in bytes
 */
uint16_t hal_flash_get_page_size(void);

/**
 * @brief Erase flash pages
 * @param addr Starting address
 * @param nb_page Number of pages to erase
 * @return SUCCESS or FAIL
 */
uint8_t hal_flash_erase_page(uint32_t addr, uint8_t nb_page);

/**
 * @brief Write buffer to flash
 * @param addr Destination address in flash
 * @param buffer Source buffer
 * @param size Number of bytes to write
 * @return Number of bytes written
 */
uint32_t hal_flash_write_buffer(uint32_t addr, const uint8_t* buffer, uint32_t size);

/**
 * @brief Read buffer from flash
 * @param addr Source address in flash
 * @param buffer Destination buffer
 * @param size Number of bytes to read
 */
void hal_flash_read_buffer(uint32_t addr, uint8_t* buffer, uint32_t size);

#if defined(USE_FLASH_READ_MODIFY_WRITE) || defined(MULTISTACK)
/**
 * @brief Read-modify-write operation on flash
 * @param addr Destination address in flash
 * @param buffer Source buffer
 * @param size Number of bytes to write
 */
void hal_flash_read_modify_write(uint32_t addr, const uint8_t* buffer, uint32_t size);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SMTC_HAL_FLASH_H */
