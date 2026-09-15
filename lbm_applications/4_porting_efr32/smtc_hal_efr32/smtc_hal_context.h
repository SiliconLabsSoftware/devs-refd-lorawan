/***************************************************************************//**
 * @file hal_context.h
 * @brief Context HAL Header File
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

#ifndef SMTC_HAL_CONTEXT_H
#define SMTC_HAL_CONTEXT_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "smtc_modem_hal.h"

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

extern uint32_t linker_storage_begin;

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// Flash base address from linker-defined internal storage section
#define SMTC_FLASH_BASE_ADDR            ((uint32_t)&linker_storage_begin)

// Context storage areas
// 1 page each
#define ADDR_FLASH_MODEM_CONTEXT          (SMTC_FLASH_BASE_ADDR)
#define ADDR_FLASH_MODEM_KEY_CONTEXT      (ADDR_FLASH_MODEM_CONTEXT + 1 * FLASH_PAGE_SIZE)
#define ADDR_FLASH_SECURE_ELEMENT_CONTEXT (ADDR_FLASH_MODEM_KEY_CONTEXT + 1 * FLASH_PAGE_SIZE)
#define ADDR_FLASH_LORAWAN_CONTEXT        (ADDR_FLASH_SECURE_ELEMENT_CONTEXT + 1 * FLASH_PAGE_SIZE)
#define ADDR_FLASH_STORE_AND_FORWARD      (ADDR_FLASH_LORAWAN_CONTEXT + 1 * FLASH_PAGE_SIZE)
// TODO: Choose a optimal number of flash page for Store and Forward
#define STORE_AND_FORWARD_MAX_FLASH_PAGE  4

#define SMTC_MAX_CONTEXT_SIZE           ((4 + STORE_AND_FORWARD_MAX_FLASH_PAGE) * FLASH_PAGE_SIZE)

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief
 *   Restores the data context.
 *
 * @param[in] ctx_type
 *   Type of modem context that need to be restored.
 *
 * @param[in] offset
 *   Memory offset after ctx_type address.
 *
 * @param[out] buffer
 *   Buffer pointer to write to.
 *
 * @param[in] size
 *   Buffer size to read in bytes.
 ******************************************************************************/
void hal_context_restore(const modem_context_type_t ctx_type,
                         uint32_t offset, uint8_t *buffer, const uint32_t size);

/***************************************************************************//**
 * @brief
 *   Stores the data context.
 *
 * @param[in] ctx_type
 *   Type of modem context that need to be saved.
 *
 * @param[in] offset
 *   Memory offset after ctx_type address.
 *
 * @param[in] buffer
 *   Buffer pointer to write from.
 *
 * @param[in] size
 *   Buffer size to write in bytes.
 ******************************************************************************/
void hal_context_store(const modem_context_type_t ctx_type,
                       uint32_t offset, const uint8_t *buffer, const uint32_t size);

/***************************************************************************//**
 * @brief
 *   Erase a chosen number of NVM3 keys of a context.
 *
 * @param[in] ctx_type
 *   Type of modem context that need to be saved.
 *
 * @param[in] offset
 *   Memory offset after ctx_type address.
 *
 * @param[in] nb_keys
 *   Number of keys to erase.
 ******************************************************************************/
void hal_context_flash_pages_erase(const modem_context_type_t ctx_type,
                                   uint32_t offset, uint8_t nb_keys);

#endif /* SMTC_HAL_CONTEXT_H */
