/***************************************************************************//**
 * @file hal_spi.c
 * @brief The LBM SPI Hardware Abstraction Layer Source File
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

#include <stdint.h>
#include <stdbool.h>

#include "smtc_hal_spi.h"
#include "spidrv.h"
#ifdef SL_CATALOG_KERNEL_PRESENT
#include "cmsis_os2.h"
#endif
#include "sl_spidrv_instances.h"

// -----------------------------------------------------------------------------
//                                Static Function Declarations
// -----------------------------------------------------------------------------
static void mutex_lock(void);
static void mutex_unlock(void);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#ifdef SL_CATALOG_KERNEL_PRESENT
static osMutexId_t                hal_spi_mutex;
static const osMutexAttr_t hal_spi_mutex_attributes = {
  .name      = "HAL SPI Mutex",
  .attr_bits = osMutexRecursive | osMutexPrioInherit,
};
#endif
/*!
 * SPI handle for radio communication
 * This should be set from the application using hal_spi_set_radio_handle()
 */
static SPIDRV_Handle_t sl_radio_spi_handle = NULL;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

uint16_t hal_spi_in_out_byte(const uint32_t id, const uint16_t out_data)
{
  // Ignore ID
  (void)(id);
  if ( sl_radio_spi_handle == NULL ) {
    return 0;      // Invalid handle
  }
  mutex_lock();
  uint8_t tx_data = (uint8_t)out_data;
  uint8_t rx_data = 0;

  // Blocking SPI transfer - send and receive 1 byte
  Ecode_t status = SPIDRV_MTransferB(sl_radio_spi_handle, &tx_data, &rx_data, 1);
  mutex_unlock();
  return (status == ECODE_EMDRV_SPIDRV_OK) ? (uint16_t)rx_data : 0;
}

uint16_t hal_spi_out(const uint32_t id, const uint8_t *out_data, const uint16_t data_len)
{
  // Ignore ID
  (void)(id);
  if ( sl_radio_spi_handle == NULL ) {
    return 0;      // Invalid handle
  }
  mutex_lock();
  // Blocking SPI transfer - send data
  Ecode_t status = SPIDRV_MTransmitB(sl_radio_spi_handle, out_data, data_len);

  mutex_unlock();
  return (status == ECODE_EMDRV_SPIDRV_OK) ? data_len : 0;
}

uint16_t hal_spi_in(const uint32_t id, uint8_t *in_data, const uint16_t data_len)
{
  // Ignore ID
  (void)(id);
  if ( sl_radio_spi_handle == NULL ) {
    return 0;      // Invalid handle
  }
  mutex_lock();
  // Blocking SPI transfer - receive data
  Ecode_t status = SPIDRV_MReceiveB(sl_radio_spi_handle, in_data, data_len);

  mutex_unlock();
  return (status == ECODE_EMDRV_SPIDRV_OK) ? data_len : 0;
}

void hal_spi_init(void)
{
  sl_radio_spi_handle = sl_spidrv_eusart_radio_handle;
#ifdef SL_CATALOG_KERNEL_PRESENT
  if (hal_spi_mutex == NULL) {
    hal_spi_mutex = osMutexNew(&hal_spi_mutex_attributes);
    EFM_ASSERT(hal_spi_mutex != NULL);
  }
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void mutex_lock(void)
{
#ifdef SL_CATALOG_KERNEL_PRESENT
  osMutexAcquire(hal_spi_mutex, osWaitForever);
#endif
}

static void mutex_unlock(void)
{
#ifdef SL_CATALOG_KERNEL_PRESENT
  osMutexRelease(hal_spi_mutex);
#endif
}

/* --- EOF ------------------------------------------------------------------ */
