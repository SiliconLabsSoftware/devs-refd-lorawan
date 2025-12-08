/***************************************************************************//**
 * @file test_sl_lbm_hal_crashlog.c
 * @brief Unit tests for LBM HAL crashlog functionality
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
#include "test_sl_lbm_hal_crashlog.h"
#include "unity.h"
#include "smtc_modem_hal.h"
#include "smtc_hal_crashlog.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// Test configuration
#define ENABLE_MCU_RESET_TEST 0  // Set to 1 to enable destructive MCU reset test (WARNING!)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Test basic crashlog store and restore functionality
 */
static void test_hal_crashlog_store_restore_basic(void);

/**
 * @brief Test crashlog store with maximum size data
 */
static void test_hal_crashlog_store_max_size(void);

/**
 * @brief Test crashlog store with oversized data (boundary condition)
 */
static void test_hal_crashlog_store_oversized(void);

/**
 * @brief Test crashlog restore with empty/uninitialized data
 */
static void test_hal_crashlog_restore_empty(void);

/**
 * @brief Test crashlog store overwrite behavior
 */
static void test_hal_crashlog_store_overwrite(void);

/**
 * @brief Test crashlog edge cases (zero length, single char, special chars)
 */
static void test_hal_crashlog_edge_cases(void);

#if ENABLE_MCU_RESET_TEST
/**
 * @brief Test actual panic function (WARNING: This will reset the MCU!)
 */
static void test_smtc_modem_hal_on_panic_actual_reset(void);
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_crashlog_test(void)
{
  UNITY_BEGIN();

  TEST_MESSAGE("=== Starting LBM HAL Crashlog Tests ===");

  // Basic crashlog functionality tests
  RUN_TEST(test_hal_crashlog_store_restore_basic);
  RUN_TEST(test_hal_crashlog_store_max_size);
  RUN_TEST(test_hal_crashlog_store_oversized);
  RUN_TEST(test_hal_crashlog_restore_empty);

  // Advanced functionality tests
  RUN_TEST(test_hal_crashlog_store_overwrite);
  RUN_TEST(test_hal_crashlog_edge_cases);

#if ENABLE_MCU_RESET_TEST
  TEST_MESSAGE("WARNING: The next test will reset the MCU");
  RUN_TEST(test_sl_lbm_hal_on_panic_actual_reset);
#endif

  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void test_hal_crashlog_store_restore_basic(void)
{
  uint8_t test_data[] = "Basic test: ABC123!@#$%^";
  uint8_t test_length = strlen((char*) test_data);

  TEST_MESSAGE("Basic crashlog store test...");

  hal_crashlog_store(test_data, test_length);

  // Verify storage by restore
  uint8_t restored_data[CRASH_LOG_SIZE] = { 0 };
  uint8_t restored_length = 0;
  hal_crashlog_restore(restored_data, &restored_length);

  TEST_ASSERT_EQUAL_UINT8_MESSAGE(test_length, restored_length,
                                  "Length mismatch: stored vs restored data");
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_data, restored_data, test_length,
                                        "Data content mismatch: stored vs restored data");

  TEST_PASS_MESSAGE("Basic crashlog store test passed");
}

static void test_hal_crashlog_store_max_size(void)
{
  uint8_t test_data[CRASH_LOG_SIZE];
  uint8_t test_length;

  TEST_MESSAGE("Crashlog store with maximum size data test...");

  // Prepare data and store
  memset(test_data, 'A', CRASH_LOG_SIZE - 1);
  test_data[CRASH_LOG_SIZE - 1] = '\0';
  test_length = CRASH_LOG_SIZE - 1;

  hal_crashlog_store(test_data, test_length);

  // Restore and verify
  uint8_t restored_data[CRASH_LOG_SIZE] = { 0 };
  uint8_t restored_length = 0;

  hal_crashlog_restore(restored_data, &restored_length);

  TEST_ASSERT_EQUAL_UINT8_MESSAGE(test_length, restored_length,
                                  "Max size length mismatch: stored vs restored data");
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_data, restored_data, test_length,
                                        "Max size data content mismatch: stored vs restored data");

  TEST_PASS_MESSAGE("Crashlog store with maximum size data test passed");
}

static void test_hal_crashlog_store_oversized(void)
{
  TEST_MESSAGE("Crashlog store with oversized data test...");
  uint8_t test_data[CRASH_LOG_SIZE + 5];
  memset(test_data, 'B', sizeof(test_data) - 1);
  test_data[sizeof(test_data) - 1] = '\0';

  uint8_t test_length = sizeof(test_data) - 1;

  hal_crashlog_store(test_data, test_length);

  // Restore and verify truncation
  uint8_t restored_data[CRASH_LOG_SIZE] = { 0 };
  uint8_t restored_length = 0;

  hal_crashlog_restore(restored_data, &restored_length);

  // Should be truncated to CRASH_LOG_SIZE
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(CRASH_LOG_SIZE, restored_length,
                                  "Oversized data length mismatch: expected truncation to CRASH_LOG_SIZE");
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_data, restored_data,
                                        CRASH_LOG_SIZE,
                                        "Oversized data content mismatch: truncated data should match first CRASH_LOG_SIZE bytes");

  TEST_PASS_MESSAGE("Oversized crashlog test passed");
}

static void test_hal_crashlog_restore_empty(void)
{
  TEST_MESSAGE("Crashlog restore with empty/uninitialized data test...");

  // Clear any existing crashlog by storing empty data
  uint8_t empty_data[] = "";
  hal_crashlog_store(empty_data, 0);

  // Try to restore
  uint8_t restored_data[CRASH_LOG_SIZE] = { 0xFF }; // Fill with non-zero pattern
  uint8_t restored_length = 0xFF;  // Initialize with non-zero value

  hal_crashlog_restore(restored_data, &restored_length);

  // Should return zero length for empty crashlog
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, restored_length,
                                  "Empty crashlog length mismatch: expected 0 length for empty data");

  TEST_PASS_MESSAGE(
    "Crashlog restore with empty/uninitialized data test passed");
}

static void test_hal_crashlog_store_overwrite(void)
{
  uint8_t test_data1[] = "First crashlog entry";
  uint8_t test_data2[] = "Second crashlog entry - overwrites first";
  uint8_t test_length1 = strlen((char*) test_data1);
  uint8_t test_length2 = strlen((char*) test_data2);

  TEST_MESSAGE("Crashlog store overwrite behavior test...");

  // Store first entry
  hal_crashlog_store(test_data1, test_length1);

  // Verify first entry
  uint8_t restored_data[CRASH_LOG_SIZE] = { 0 };
  uint8_t restored_length = 0;
  hal_crashlog_restore(restored_data, &restored_length);

  TEST_ASSERT_EQUAL_UINT8_MESSAGE(test_length1, restored_length,
                                  "Overwrite test first entry length mismatch: first entry should be stored correctly");
  TEST_ASSERT_EQUAL_STRING_MESSAGE((char* )test_data1, (char* )restored_data,
                                   "Overwrite test first entry content mismatch: first entry should be restored correctly");

  // Store second entry (should overwrite first)
  hal_crashlog_store(test_data2, test_length2);

  // Verify second entry overwrote first
  memset(restored_data, 0, sizeof(restored_data));
  restored_length = 0;
  hal_crashlog_restore(restored_data, &restored_length);

  TEST_ASSERT_EQUAL_UINT8_MESSAGE(test_length2, restored_length,
                                  "Overwrite test length mismatch: second entry length should match");
  TEST_ASSERT_EQUAL_STRING_MESSAGE((char* )test_data2, (char* )restored_data,
                                   "Overwrite test content mismatch: second entry should overwrite first");

  TEST_PASS_MESSAGE("Crashlog store overwrite behavior test passed");
}

static void test_hal_crashlog_edge_cases(void)
{
  TEST_MESSAGE("Crashlog edge cases test...");

  // Test 1: Single character
  uint8_t single_char[] = "A";
  hal_crashlog_store(single_char, 1);

  uint8_t restored_data[CRASH_LOG_SIZE] = { 0 };
  uint8_t restored_length = 0;
  hal_crashlog_restore(restored_data, &restored_length);

  TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, restored_length,
                                  "Edge case single char length mismatch: should handle single character data");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE('A', restored_data[0],
                                  "Edge case single char content mismatch: should preserve single character");

  // Test 2: String with null characters in the middle
  uint8_t null_string[] = { 'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l',
                            'd' };
  hal_crashlog_store(null_string, sizeof(null_string));

  memset(restored_data, 0, sizeof(restored_data));
  restored_length = 0;
  hal_crashlog_restore(restored_data, &restored_length);

  TEST_ASSERT_EQUAL_UINT8_MESSAGE(sizeof(null_string), restored_length,
                                  "Edge case null string length mismatch: should handle embedded null characters");
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(null_string, restored_data,
                                        sizeof(null_string),
                                        "Edge case null string content mismatch: should preserve embedded null characters");

  TEST_PASS_MESSAGE("Crashlog edge cases test passed");
}

#if ENABLE_MCU_RESET_TEST
static void test_smtc_modem_hal_on_panic_actual_reset(void)
{
  TEST_MESSAGE("WARNING: This test will reset the MCU !");

  uint8_t *test_func = (uint8_t*)"test_reset_function";
  smtc_modem_hal_on_panic(test_func, 999, "Intentional test reset: %s", "MCU_RESET_TEST");

  // This line should never be reached
  TEST_FAIL_MESSAGE("MCU reset did not occur - panic function failed!");
}
#endif
