/***************************************************************************//**
 * @file hal_flash.c
 * @brief Flash Hardware Abstraction Layer for LoRaWAN Basic Modem
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

#include "smtc_hal_flash.h"
#include "em_msc.h"
#include "em_core.h"
#include <string.h>
#include "em_cmu.h"
#include "smtc_hal_dbg_trace.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief  Gets the page number of a given address
 * @param  addr: Address of the FLASH Memory
 * @retval The page number of a given address
 */
static uint32_t hal_flash_get_page(uint32_t addr);
static void mutex_lock(void);
static void mutex_unlock(void);
// -----------------------------------------------------------------------------
//                          Static variable Definitions
// -----------------------------------------------------------------------------

#ifdef SL_CATALOG_KERNEL_PRESENT
static osMutexId_t                hal_flash_mutex;
static const osMutexAttr_t hal_flash_mutex_attributes = {
  .name      = "HAL FLASH Mutex",
  .attr_bits = osMutexRecursive | osMutexPrioInherit,
};
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

uint16_t hal_flash_get_page_size(void)
{
  return FLASH_PAGE_SIZE;
}

uint8_t hal_flash_erase_page(uint32_t addr, uint8_t nb_page)
{
  mutex_lock();
  // Validate address range
  if ((addr + (FLASH_PAGE_SIZE * (nb_page - 1))) > (FLASH_BASE + FLASH_SIZE)) {
    SMTC_HAL_TRACE_ERROR("FLASH_OPERATION_BAD PAGE ADDR\r\n");
    return FAIL;
  }

  // Calculate page-aligned start address
  uint32_t first_page = hal_flash_get_page(addr);
  uint32_t start_addr_page = FLASH_BASE + (FLASH_PAGE_SIZE * first_page);

  for (uint16_t i = 0; i < nb_page; i++) {
    MSC_Status_TypeDef status = MSC_ErasePage((uint32_t*)start_addr_page);
    if (status != mscReturnOk) {
      SMTC_HAL_TRACE_ERROR("Failed to erase page at 0x%08lX\r\n", start_addr_page);
      return FAIL;
    }
    start_addr_page += FLASH_PAGE_SIZE;
  }
  mutex_unlock();
  return SUCCESS;
}

uint32_t hal_flash_write_buffer(uint32_t addr, const uint8_t* buffer, uint32_t size)
{
  if (buffer == NULL || size == 0) {
    return 0;
  }
  mutex_lock();

  MSC_Status_TypeDef status = MSC_WriteWord((uint32_t*)addr, buffer, size);

  if (status != mscReturnOk) {
    SMTC_HAL_TRACE_ERROR("Failed to write to flash at 0x%08lX\r\n", addr);
    return 0;
  }
  mutex_unlock();
  return size;
}

void hal_flash_read_buffer(uint32_t addr, uint8_t* buffer, uint32_t size)
{
  if (buffer == NULL || size == 0) {
    return;
  }

  // Direct memory copy from flash
  memcpy(buffer, (const uint8_t*)addr, size);
}

#if defined(USE_FLASH_READ_MODIFY_WRITE) || defined(MULTISTACK)
static uint8_t copy_page[FLASH_PAGE_SIZE] = { 0x00 };  // Use max page size as fallback

void hal_flash_read_modify_write(uint32_t addr, const uint8_t* buffer, uint32_t size)
{
  if (buffer == NULL || size == 0) {
    return;
  }
  mutex_lock();
  uint32_t remaining_size = size;

  do {
    memset(copy_page, 0xFF, sizeof(copy_page));

    // Get the page number
    uint32_t num_page = hal_flash_get_page(addr);

    // Get start address of this flash page
    uint32_t start_addr_page = FLASH_BASE + (FLASH_PAGE_SIZE * num_page);

    // Read data from this flash page
    hal_flash_read_buffer(start_addr_page, copy_page, FLASH_PAGE_SIZE);

    // Compute the index where data needs to be updated in RAM copy page
    uint32_t index = (addr - start_addr_page) % FLASH_PAGE_SIZE;

    // Compute the size of the data to be copied without overflow to the next page
    uint32_t cpy_size = (remaining_size < (FLASH_PAGE_SIZE - index)) ? remaining_size : (FLASH_PAGE_SIZE - index);
    memcpy(&copy_page[index], &buffer[size - remaining_size], cpy_size);

    // Erase flash page
    hal_flash_erase_page(start_addr_page, 1);

    // Write the RAM buffer to this flash page
    hal_flash_write_buffer(start_addr_page, copy_page, FLASH_PAGE_SIZE);

    // Increment address to the next page
    addr += FLASH_PAGE_SIZE - index;

    // Reduce the amount of data remaining to be written
    remaining_size -= cpy_size;
  } while (remaining_size != 0);
  mutex_unlock();
}
#endif  // USE_FLASH_READ_MODIFY_WRITE or MULTISTACK

void hal_flash_init(void)
{
  CMU_ClockEnable(cmuClock_MSC, true);
  MSC_Init();
#ifdef SL_CATALOG_KERNEL_PRESENT
  if (hal_flash_mutex == NULL) {
    hal_flash_mutex = osMutexNew(&hal_flash_mutex_attributes);
    EFM_ASSERT(hal_flash_mutex != NULL);
  }
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static uint32_t hal_flash_get_page(uint32_t addr)
{
  return (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void mutex_lock(void)
{
#ifdef SL_CATALOG_KERNEL_PRESENT
  osMutexAcquire(hal_flash_mutex, osWaitForever);
#endif
}

static void mutex_unlock(void)
{
#ifdef SL_CATALOG_KERNEL_PRESENT
  osMutexRelease(hal_flash_mutex);
#endif
}
