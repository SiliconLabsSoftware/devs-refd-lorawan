/***************************************************************************//**
 * @file hal_bootloader.c
 * @brief The LBM Bootloader Handle Source File
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
#include <string.h>
#include <stdlib.h>
#include "smtc_hal_bootloader.h"
#include "app_log.h"
#include "smtc_modem_hal.h"
#include "smtc_hal_mcu.h"
#include "btl_interface.h"
#include "btl_interface_storage.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static BootloaderInformation_t bootloader_info;
static BootloaderStorageSlot_t slot_info;
// Allocate buffer for one page (assuming 0x2000 bytes page size)
static uint8_t page_buffer[FLASH_PAGE_SIZE];
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void hal_bootloader_read_storage(uint32_t slotId, uint32_t offset, uint8_t* buffer, uint32_t size)
{
  if (buffer == NULL || size == 0) {
    app_log_error("Invalid parameters in hal_bootloader_read_storage\n");
    return;
  }

  int32_t err = bootloader_readStorage(slotId, offset, buffer, size);
  if (err != BOOTLOADER_OK) {
    app_log_error("Error reading from flash at offset %d with error: %d\n", offset, err);
  }
}

int32_t hal_bootloader_get_slot_info()
{
  int32_t err;

  bootloader_getInfo(&bootloader_info);

  app_log_info("Bootloader version: %u.%u\r\n", (bootloader_info.version & 0xFF000000) >> 24, (bootloader_info.version & 0x00FF0000) >> 16);

  err = bootloader_getStorageSlotInfo(BOOTLOADER_STORAGE_SLOT, &slot_info);

  if (err == BOOTLOADER_OK) {
    app_log_info("Slot start @ 0x%8.8x, size %u bytes\r\n", slot_info.address, slot_info.length);
  } else {
    app_log_error("Failed to get storage slot info: %u\r\n", err);
  }

  return err;
}

bool hal_bootloader_verify_application(void)
{
  int32_t err;
  bool image_verified = false;

  err = bootloader_verifyImage(BOOTLOADER_STORAGE_SLOT, NULL);

  if (err != BOOTLOADER_OK) {
    app_log_error("Application verification failed: %u \r\n", err);
  } else {
    app_log_info("Application verified\n");
    image_verified = true;
  }

  return image_verified;
}

void hal_bootloader_erase_slot_if_needed(void)
{
  uint32_t offset = 0;
  uint32_t num_blocks = 0;
  uint8_t buffer[256];
  bool dirty = false;
  int32_t err = BOOTLOADER_OK;

  // Check the download area content by reading it in 256-bytes blocks
  num_blocks = slot_info.length / 256;

  while ((dirty == 0) && (offset < 256 * num_blocks) && (err == BOOTLOADER_OK)) {
    err = bootloader_readStorage(BOOTLOADER_STORAGE_SLOT, offset, buffer, 256);
    if (err != BOOTLOADER_OK) {
      break;
    }
    // Check if any byte in the buffer is not 0xFF
    for (uint32_t i = 0; i < 256; i++) {
      if (buffer[i] != 0xFF) {
        dirty = true;
        break;
      }
    }
    offset += 256;
    app_log_info(".");
  }

  if (err != BOOTLOADER_OK) {
    app_log_error("Failed to read storage: %u\r\n", err);
  } else if (dirty) {
    app_log_info("Download area is not empty, erasing...\r\n");
    bootloader_eraseStorageSlot(0);
    app_log_info("DONE!\r\n");
  } else {
    app_log_info("Download area is empty, no need to erase.\r\n");
  }
}

void hal_bootloader_read_modify_write(uint32_t slotId, uint32_t offset, const uint8_t* buffer, uint32_t size)
{
  if (buffer == NULL || size == 0) {
    app_log_error("Invalid parameters in hal_bootloader_read_modify_write\n");
    return;
  }

  int32_t err = bootloader_writeStorage(slotId, offset, buffer, size);
  if (err == BOOTLOADER_OK) {
    return;
  }

  app_log_warning("Direct write failed, attempting read-modify-write at offset %d size %d, error: %ld\n", offset, size, err);
  // Get storage slot information
  BootloaderStorageSlot_t slotInfo;
  if (bootloader_getStorageSlotInfo(slotId, &slotInfo) != BOOTLOADER_OK) {
    app_log_error("Failed to get storage slot info for slot %lu\n", slotId);
    return;
  }

  uint32_t remaining_size = size;
  uint32_t current_offset = offset;
  uint32_t buffer_index = 0;

  do {
    // Calculate which page the current offset falls into
    uint32_t page_start_offset = (current_offset / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
    uint32_t offset_in_page = current_offset - page_start_offset;

    // Calculate how much data to write in this page
    uint32_t bytes_to_write = (remaining_size < (FLASH_PAGE_SIZE - offset_in_page))
                              ? remaining_size : (FLASH_PAGE_SIZE - offset_in_page);

    // Read entire page into buffer
    memset(page_buffer, 0xFF, FLASH_PAGE_SIZE);
    err = bootloader_readStorage(slotId, page_start_offset, page_buffer, FLASH_PAGE_SIZE);
    if (err != BOOTLOADER_OK) {
      app_log_error("Failed to read storage page at offset %lu, error: %ld\n", page_start_offset, err);
      return;
    }

    // Modify only the required bytes
    memcpy(&page_buffer[offset_in_page], &buffer[buffer_index], bytes_to_write);

    // Erase and write back the entire page
    BootloaderEraseStatus_t eraseStat;
    if (bootloader_initChunkedEraseStorageSlot(slotId, &eraseStat) == BOOTLOADER_OK) {
      // Move to the correct page
      eraseStat.currentPageAddr = slotInfo.address + page_start_offset;
      bootloader_chunkedEraseStorageSlot(&eraseStat);
    }

    // Write the modified page back
    err = bootloader_writeStorage(slotId, page_start_offset, page_buffer, FLASH_PAGE_SIZE);
    if (err != BOOTLOADER_OK) {
      app_log_error("Failed to write storage page at offset %lu, error: %ld\n", page_start_offset, err);
      return;
    }

    // Move to next page
    current_offset += bytes_to_write;
    buffer_index += bytes_to_write;
    remaining_size -= bytes_to_write;
  } while (remaining_size > 0);

  app_log_info("FUOTA read-modify-write completed successfully at offset %d\n", offset);
}

void hal_bootloader_setImage_and_reboot(void)
{
  app_log_info("Rebooting to new application\n");
  bootloader_setImageToBootload(BOOTLOADER_STORAGE_SLOT);
  bootloader_rebootAndInstall();
}
