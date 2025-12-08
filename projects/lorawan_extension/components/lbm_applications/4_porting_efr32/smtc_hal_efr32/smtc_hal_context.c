/***************************************************************************//**
 * @file hal_context.c
 * @brief The LBM Context Saving Source File
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "smtc_hal_context.h"
#include "smtc_hal_flash.h"
#include "app_log.h"
#include "smtc_modem_hal.h"
#include "smtc_hal_mcu.h"
#if defined (USE_FUOTA)
#include "smtc_hal_bootloader.h"
#endif
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

__attribute__((used)) uint8_t internal_storage[SMTC_MAX_CONTEXT_SIZE]
__attribute__ ((section(".internal_storage")));

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void hal_context_restore(const modem_context_type_t ctx_type,
                         uint32_t offset, uint8_t *buffer, const uint32_t size)
{
  switch ( ctx_type ) {
    case CONTEXT_MODEM:
      hal_flash_read_buffer(ADDR_FLASH_MODEM_CONTEXT, buffer, size);
      break;
    case CONTEXT_KEY_MODEM:
      hal_flash_read_buffer(ADDR_FLASH_MODEM_KEY_CONTEXT, buffer, size);
      break;
    case CONTEXT_LORAWAN_STACK:
      hal_flash_read_buffer(ADDR_FLASH_LORAWAN_CONTEXT + offset, buffer, size);
      break;
    case CONTEXT_FUOTA:
#if defined (USE_FUOTA)
      app_log_info("FUOTA restoring at offset: %d\n", offset);
      hal_bootloader_read_storage(BOOTLOADER_STORAGE_SLOT, offset, buffer, size);
#else
      app_log_error("FUOTA not enabled, cannot restore context\n");
#endif
      break;
    case CONTEXT_SECURE_ELEMENT:
      hal_flash_read_buffer(ADDR_FLASH_SECURE_ELEMENT_CONTEXT, buffer, size);
      break;
    case CONTEXT_STORE_AND_FORWARD:
      hal_flash_read_buffer(ADDR_FLASH_STORE_AND_FORWARD + offset, buffer, size);
      break;
    default:
      mcu_panic( );
      break;
  }
}

void hal_context_store(const modem_context_type_t ctx_type,
                       uint32_t offset, const uint8_t *buffer, const uint32_t size)
{
  switch ( ctx_type ) {
    case CONTEXT_MODEM:
      hal_flash_erase_page(ADDR_FLASH_MODEM_CONTEXT, 1);
      hal_flash_write_buffer(ADDR_FLASH_MODEM_CONTEXT, buffer, size);
      break;
    case CONTEXT_KEY_MODEM:
      hal_flash_erase_page(ADDR_FLASH_MODEM_KEY_CONTEXT, 1);
      hal_flash_write_buffer(ADDR_FLASH_MODEM_KEY_CONTEXT, buffer, size);
      break;
    case CONTEXT_LORAWAN_STACK:
#if defined(MULTISTACK)
      // In case code is built for multiple stacks, read_modify_write feature is mandatory
      hal_flash_read_modify_write(ADDR_FLASH_LORAWAN_CONTEXT + offset, buffer, size);
#else
      hal_flash_erase_page(ADDR_FLASH_LORAWAN_CONTEXT, 1);
      hal_flash_write_buffer(ADDR_FLASH_LORAWAN_CONTEXT, buffer, size);
#endif
      break;
    case CONTEXT_FUOTA:
#if defined (USE_FUOTA)
      app_log_info("FUOTA storing at offset: %d", offset);
      hal_bootloader_read_modify_write(BOOTLOADER_STORAGE_SLOT, offset, buffer, size);
#else
      app_log_error("FUOTA not enabled, cannot store context\n");
#endif
      break;
    case CONTEXT_SECURE_ELEMENT:
      hal_flash_erase_page(ADDR_FLASH_SECURE_ELEMENT_CONTEXT, 1);
      hal_flash_write_buffer(ADDR_FLASH_SECURE_ELEMENT_CONTEXT, buffer, size);
      break;
    case CONTEXT_STORE_AND_FORWARD:
      hal_flash_write_buffer(ADDR_FLASH_STORE_AND_FORWARD + offset, buffer, size);
      break;
    default:
      mcu_panic( );
      break;
  }
}

void hal_context_flash_pages_erase(const modem_context_type_t ctx_type,
                                   uint32_t offset, uint8_t nb_page)
{
  // Invalid parameters
  if (ctx_type != CONTEXT_STORE_AND_FORWARD || nb_page == 0) {
    if (ctx_type != CONTEXT_STORE_AND_FORWARD) {
      app_log_debug("Page erase only supported for Store&Forward context\n");
    }
    return;
  }

  // Limit pages to maximum allowed
  nb_page = (nb_page > STORE_AND_FORWARD_MAX_FLASH_PAGE)
            ? STORE_AND_FORWARD_MAX_FLASH_PAGE : nb_page;
  app_log_debug("Erasing Store and Forward: %d pages from offset %lu\n", nb_page, offset);

  hal_flash_erase_page(ADDR_FLASH_STORE_AND_FORWARD + offset, nb_page);
}
