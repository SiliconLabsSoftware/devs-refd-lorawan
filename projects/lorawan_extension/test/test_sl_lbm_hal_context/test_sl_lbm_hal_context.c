/***************************************************************************//**
 * @brief Unit tests for LBM HAL Context Management
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
#include "test_sl_lbm_hal_context.h"
#include "smtc_hal_context.h"
#include "unity.h"
#include "smtc_modem_hal.h"
#include "nvm3_generic.h"
#include <string.h>
#include <stdio.h>
#include "sl_status.h"
#include "smtc_hal_flash.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define TEST_BUFFER_SIZE              64
#define TEST_LARGE_BUFFER_SIZE        512
#define TEST_PATTERN_0xAA             0xAA
#define TEST_PATTERN_0x55             0x55
#define TEST_PATTERN_0xCC             0xCC
#define TEST_PATTERN_0x33             0x33
#define TEST_OFFSET                   1024
#define TEST_FLASH_PAGES_COUNT        5

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_hal_context_store_restore_single_page(void);
static void test_hal_context_store_restore_multiple_page(void);
static void test_hal_context_restore_from_erased_flash(void);
static void test_hal_context_boundary_conditions(void);
static void test_hal_context_store_data_overwrite(void);
static void test_hal_context_flash_pages_erase(void);

// Helper functions
static void sli_fill_test_buffer(uint8_t *buffer, uint32_t size,
                                 uint8_t pattern);
static bool sli_verify_test_buffer(const uint8_t *buffer, uint32_t size,
                                   uint8_t pattern);
static bool sli_verify_buffer_is_erased_flash(const uint8_t *buffer, uint32_t size);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_context_test(void)
{
  UNITY_BEGIN();

  // Initialize MSC for testing
  hal_flash_init();

  RUN_TEST(test_hal_context_store_restore_single_page);
  RUN_TEST(test_hal_context_store_restore_multiple_page);
  RUN_TEST(test_hal_context_restore_from_erased_flash);
  RUN_TEST(test_hal_context_boundary_conditions);
  RUN_TEST(test_hal_context_store_data_overwrite);
  RUN_TEST(test_hal_context_flash_pages_erase);

  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test basic context store restore operations for single page data with context isolation
 */
static void test_hal_context_store_restore_single_page(void)
{
  uint8_t store_buffer_modem[TEST_BUFFER_SIZE];
  uint8_t store_buffer_key[TEST_BUFFER_SIZE];
  uint8_t store_buffer_secure[TEST_BUFFER_SIZE];
  uint8_t store_buffer_stack[TEST_BUFFER_SIZE];

  uint8_t restore_buffer_modem[TEST_BUFFER_SIZE];
  uint8_t restore_buffer_key[TEST_BUFFER_SIZE];
  uint8_t restore_buffer_secure[TEST_BUFFER_SIZE];
  uint8_t restore_buffer_stack[TEST_BUFFER_SIZE];

  // Step 1: Store different patterns in all single-page contexts
  sli_fill_test_buffer(store_buffer_modem, sizeof(store_buffer_modem), TEST_PATTERN_0xAA);
  sli_fill_test_buffer(store_buffer_key, sizeof(store_buffer_key), TEST_PATTERN_0x55);
  sli_fill_test_buffer(store_buffer_secure, sizeof(store_buffer_secure), TEST_PATTERN_0xCC);
  sli_fill_test_buffer(store_buffer_stack, sizeof(store_buffer_stack), TEST_PATTERN_0x33);

  hal_context_store(CONTEXT_MODEM, 0, store_buffer_modem, sizeof(store_buffer_modem));
  hal_context_store(CONTEXT_KEY_MODEM, 0, store_buffer_key, sizeof(store_buffer_key));
  hal_context_store(CONTEXT_SECURE_ELEMENT, 0, store_buffer_secure, sizeof(store_buffer_secure));
  hal_context_store(CONTEXT_LORAWAN_STACK, 0, store_buffer_stack, sizeof(store_buffer_stack));

  // Step 2: Restore and verify all contexts maintain their data independently
  memset(restore_buffer_modem, 0, sizeof(restore_buffer_modem));
  memset(restore_buffer_key, 0, sizeof(restore_buffer_key));
  memset(restore_buffer_secure, 0, sizeof(restore_buffer_secure));
  memset(restore_buffer_stack, 0, sizeof(restore_buffer_stack));

  hal_context_restore(CONTEXT_MODEM, 0, restore_buffer_modem, sizeof(restore_buffer_modem));
  hal_context_restore(CONTEXT_KEY_MODEM, 0, restore_buffer_key, sizeof(restore_buffer_key));
  hal_context_restore(CONTEXT_SECURE_ELEMENT, 0, restore_buffer_secure, sizeof(restore_buffer_secure));
  hal_context_restore(CONTEXT_LORAWAN_STACK, 0, restore_buffer_stack, sizeof(restore_buffer_stack));

  // Step 3: Verify data integrity and context isolation
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer_modem, sizeof(restore_buffer_modem), TEST_PATTERN_0xAA),
    "CONTEXT_MODEM: Restored data should match stored data pattern 0xAA");

  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer_key, sizeof(restore_buffer_key), TEST_PATTERN_0x55),
    "CONTEXT_KEY_MODEM: Restored data should match stored data pattern 0x55");

  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer_secure, sizeof(restore_buffer_secure), TEST_PATTERN_0xCC),
    "CONTEXT_SECURE_ELEMENT: Restored data should match stored data pattern 0xCC");

  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer_stack, sizeof(restore_buffer_stack), TEST_PATTERN_0x33),
    "CONTEXT_LORAWAN_STACK: Restored data should match stored data pattern 0x33");

  TEST_PASS_MESSAGE("Single page context store-restore with isolation test completed successfully");
}

/**
 * @brief Test basic context store restore operations for multiple pages with context isolation
 */
static void test_hal_context_store_restore_multiple_page(void)
{
  uint8_t store_buffer_forward[TEST_LARGE_BUFFER_SIZE];
  uint8_t restore_buffer_forward[TEST_LARGE_BUFFER_SIZE];

  // Step 1: Store different patterns in all multi-page contexts
  sli_fill_test_buffer(store_buffer_forward, sizeof(store_buffer_forward), TEST_PATTERN_0x55);

  hal_context_store(CONTEXT_STORE_AND_FORWARD, TEST_OFFSET, store_buffer_forward, sizeof(store_buffer_forward));

  // Step 2: Restore and verify both contexts maintain their data independently
  memset(restore_buffer_forward, 0, sizeof(restore_buffer_forward));
  hal_context_restore(CONTEXT_STORE_AND_FORWARD, TEST_OFFSET, restore_buffer_forward, sizeof(restore_buffer_forward));

  // Step 3: Verify data integrity and context isolation
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer_forward, sizeof(restore_buffer_forward), TEST_PATTERN_0x55),
    "CONTEXT_STORE_AND_FORWARD: Restored data should match stored data pattern 0x55");

  TEST_PASS_MESSAGE("Multiple page context store-restore with isolation test completed successfully");
}

/**
 * @brief Test context restore behavior when reading from erased flash
 */
static void test_hal_context_restore_from_erased_flash(void)
{
  uint8_t restore_buffer[TEST_BUFFER_SIZE];

  memset(restore_buffer, 0, sizeof(restore_buffer));

  // Read from a context that hasn't been written to yet
  hal_context_restore(CONTEXT_LORAWAN_STACK, TEST_OFFSET, restore_buffer, sizeof(restore_buffer));

  // MSC flash erased state is 0xFF
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_buffer_is_erased_flash(restore_buffer, sizeof(restore_buffer)),
    "CONTEXT_LORAWAN_STACK: Uninitialized flash should read as 0xFF (erased state)");

  TEST_PASS_MESSAGE("Restore from erased flash verified - buffer contains 0xFF pattern");
}

/**
 * @brief Test boundary conditions and edge cases
 */
static void test_hal_context_boundary_conditions(void)
{
  uint8_t test_buffer[TEST_BUFFER_SIZE];
  uint8_t restore_buffer[TEST_BUFFER_SIZE];

  // Test with NULL buffer
  hal_context_store(CONTEXT_MODEM, 0, NULL, sizeof(test_buffer));
  hal_context_restore(CONTEXT_MODEM, 0, NULL, sizeof(restore_buffer));

  // Test with zero size
  sli_fill_test_buffer(test_buffer, sizeof(test_buffer), TEST_PATTERN_0xAA);
  hal_context_store(CONTEXT_MODEM, 0, test_buffer, 0);

  sli_fill_test_buffer(restore_buffer, sizeof(restore_buffer), TEST_PATTERN_0x55);
  hal_context_restore(CONTEXT_MODEM, 0, restore_buffer, 0);

  // Buffer should remain unchanged with size=0
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer, sizeof(restore_buffer), TEST_PATTERN_0x55),
    "Zero-size operations should not modify buffers");

  TEST_PASS_MESSAGE("Boundary condition tests completed");
}

static void test_hal_context_store_data_overwrite(void)
{
  uint8_t buffer[TEST_BUFFER_SIZE], restore[TEST_BUFFER_SIZE];

  // Store first pattern
  sli_fill_test_buffer(buffer, TEST_BUFFER_SIZE, TEST_PATTERN_0xAA);
  hal_context_store(CONTEXT_MODEM, 0, buffer, TEST_BUFFER_SIZE);

  // Verify first pattern was stored correctly
  memset(restore, 0, TEST_BUFFER_SIZE);
  hal_context_restore(CONTEXT_MODEM, 0, restore, TEST_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore, TEST_BUFFER_SIZE, TEST_PATTERN_0xAA),
    "First pattern (0xAA) should be stored correctly");

  // Store different pattern (should overwrite)
  sli_fill_test_buffer(buffer, TEST_BUFFER_SIZE, TEST_PATTERN_0x55);
  hal_context_store(CONTEXT_MODEM, 0, buffer, TEST_BUFFER_SIZE);

  // Verify only new pattern exists
  memset(restore, 0, TEST_BUFFER_SIZE);
  hal_context_restore(CONTEXT_MODEM, 0, restore, TEST_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore, TEST_BUFFER_SIZE, TEST_PATTERN_0x55),
    "Second pattern (0x55) should overwrite first pattern");

  TEST_PASS_MESSAGE("Store data overwrite tests completed");
}

/**
 * @brief Test flash page erase functionality
 */
static void test_hal_context_flash_pages_erase(void)
{
  uint8_t store_buffer[TEST_LARGE_BUFFER_SIZE];
  uint8_t restore_buffer[TEST_LARGE_BUFFER_SIZE];

  sli_fill_test_buffer(store_buffer, sizeof(store_buffer), TEST_PATTERN_0xAA);

  // Store data in two different locations
  hal_context_store(CONTEXT_STORE_AND_FORWARD, 0, store_buffer, sizeof(store_buffer));
  hal_context_store(CONTEXT_STORE_AND_FORWARD, TEST_LARGE_BUFFER_SIZE, store_buffer, sizeof(store_buffer));

  // Verify first location has data before erase
  memset(restore_buffer, 0, sizeof(restore_buffer));
  hal_context_restore(CONTEXT_STORE_AND_FORWARD, 0, restore_buffer, sizeof(restore_buffer));
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer, sizeof(restore_buffer), TEST_PATTERN_0xAA),
    "Data should be stored before erase");

  // Verify second location has data before erase
  memset(restore_buffer, 0, sizeof(restore_buffer));
  hal_context_restore(CONTEXT_STORE_AND_FORWARD, TEST_LARGE_BUFFER_SIZE, restore_buffer, sizeof(restore_buffer));
  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_test_buffer(restore_buffer, sizeof(restore_buffer), TEST_PATTERN_0xAA),
    "Data should be stored at second location before erase");

  // Erase pages
  hal_context_flash_pages_erase(CONTEXT_STORE_AND_FORWARD, 0, TEST_FLASH_PAGES_COUNT);

  // Verify data is erased (should read as 0xFF) at first location
  memset(restore_buffer, 0, sizeof(restore_buffer));
  hal_context_restore(CONTEXT_STORE_AND_FORWARD, 0, restore_buffer, sizeof(restore_buffer));

  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_buffer_is_erased_flash(restore_buffer, sizeof(restore_buffer)),
    "Data should be erased (0xFF) after page erase operation at first location");

  // Verify data is erased (should read as 0xFF) at second location
  memset(restore_buffer, 0, sizeof(restore_buffer));
  hal_context_restore(CONTEXT_STORE_AND_FORWARD, TEST_LARGE_BUFFER_SIZE, restore_buffer, sizeof(restore_buffer));

  TEST_ASSERT_TRUE_MESSAGE(
    sli_verify_buffer_is_erased_flash(restore_buffer, sizeof(restore_buffer)),
    "Data should be erased (0xFF) after page erase operation at second location");

  TEST_PASS_MESSAGE("Flash page erase operations completed successfully");
}

// -----------------------------------------------------------------------------
//                          Helper Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Fill buffer with test pattern
 */
static void sli_fill_test_buffer(uint8_t *buffer, uint32_t size,
                                 uint8_t pattern)
{
  for (uint32_t i = 0; i < size; i++) {
    buffer[i] = pattern;
  }
}

/**
 * @brief Verify buffer contains expected pattern
 */
static bool sli_verify_test_buffer(const uint8_t *buffer, uint32_t size,
                                   uint8_t pattern)
{
  for (uint32_t i = 0; i < size; i++) {
    if (buffer[i] != pattern) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Verify buffer contains erased flash pattern (0xFF)
 */
static bool sli_verify_buffer_is_erased_flash(const uint8_t *buffer, uint32_t size)
{
  return sli_verify_test_buffer(buffer, size, 0xFF);
}
