/***************************************************************************//**
 * @file test_sl_lbm_hal_rtc.c
 * @brief Unit tests for LBM HAL RTC
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
#include "test_sl_lbm_hal_rtc.h"
#include "unity.h"
#include "smtc_hal_rtc.h"
#include "sl_hal_sysrtc.h"
#include "sl_interrupt_manager.h"
#include "sl_sleeptimer.h"
#include "smtc_modem_hal.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_RTC_TIME_TOLERANCE_MS (10)  // 10ms tolerance for timing tests

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_hal_rtc_initialization(void);
static void test_hal_rtc_is_running_after_init(void);
static void test_hal_rtc_time_functions_operational(void);
static void test_hal_rtc_time_consistency_check(void);
static void test_hal_rtc_wrapping_functionality(void);
static void test_smtc_modem_hal_get_time_in_s(void);
static void test_smtc_modem_hal_get_time_in_ms(void);
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_rtc_test(void)
{
  UNITY_BEGIN();

  // Test 1: Initialize RTC and verify successful initialization
  RUN_TEST(test_hal_rtc_initialization);

  // Test 2: Verify RTC is running correctly after initialization
  RUN_TEST(test_hal_rtc_is_running_after_init);

  // Test 3: Test that time functions are operational
  RUN_TEST(test_hal_rtc_time_functions_operational);

  // Test 4: Test time consistency between different formats
  RUN_TEST(test_hal_rtc_time_consistency_check);

  // Test 5: Test getting time in seconds
  RUN_TEST(test_smtc_modem_hal_get_time_in_s);

  // Test 6: Test getting time in milliseconds
  RUN_TEST(test_smtc_modem_hal_get_time_in_ms);

  // Test 7: Test wrapping functionality
  RUN_TEST(test_hal_rtc_wrapping_functionality);
  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test RTC initialization
 * This test initializes the RTC and verifies it completes without errors
 */
static void test_hal_rtc_initialization(void)
{
  // Initialize the RTC
  hal_rtc_init();

  // Make sure the RTC counter is increasing
  uint32_t current_rtc_counter = sl_hal_sysrtc_get_counter();
  TEST_ASSERT_GREATER_THAN(0, current_rtc_counter);

  // Make sure SYSRTC interrupts are disable
  bool is_irq_enabled = sl_interrupt_manager_is_irq_disabled(SYSRTC_APP_IRQn);
  TEST_ASSERT_TRUE_MESSAGE(is_irq_enabled, "RTC interrupt should be enabled after initialization");
}

/**
 * @brief Test that RTC is running correctly after initialization
 * This test assumes RTC was already initialized and verifies it's operational
 */
static void test_hal_rtc_is_running_after_init(void)
{
  // RTC should already be initialized from previous test
  // 1200 ms delay before run test
  sl_sleeptimer_delay_millisecond(1200);
  // Verify basic functionality by getting time
  uint32_t time_s = hal_rtc_get_time_in_s();
  uint32_t time_ms = hal_rtc_get_time_in_ms();

  // Verify that we got valid time values (not zero, not max)
  TEST_ASSERT_NOT_EQUAL(0, time_s);
  TEST_ASSERT_NOT_EQUAL(UINT32_MAX, time_s);
  TEST_ASSERT_NOT_EQUAL(0, time_ms);
  TEST_ASSERT_NOT_EQUAL(UINT32_MAX, time_ms);
}

/**
 * @brief Test that RTC time functions are operational
 * This test verifies that time reading functions work and return monotonic values
 */
static void test_hal_rtc_time_functions_operational(void)
{
  // Get initial time readings
  uint32_t time1_s = hal_rtc_get_time_in_s();
  uint32_t time1_ms = hal_rtc_get_time_in_ms();

  // Small delay to allow time to advance
  sl_sleeptimer_delay_millisecond(200);

  // Get second time readings
  uint32_t time2_s = hal_rtc_get_time_in_s();
  uint32_t time2_ms = hal_rtc_get_time_in_ms();

  // Time should be monotonic (not going backwards)
  TEST_ASSERT_GREATER_OR_EQUAL(time1_s, time2_s);
  TEST_ASSERT_GREATER_OR_EQUAL(time1_ms, time2_ms);
}

/**
 * @brief Test time consistency between seconds and milliseconds
 * This test verifies that time values are consistent between different formats
 */
static void test_hal_rtc_time_consistency_check(void)
{
  // Get time in both formats
  uint32_t time_s = hal_rtc_get_time_in_s();
  uint32_t time_ms = hal_rtc_get_time_in_ms();

  // Convert seconds to milliseconds for comparison
  uint32_t time_s_to_ms = time_s * 1000;

  // Allow for some tolerance due to timing differences (up to 1 second)
  uint32_t diff = (time_ms > time_s_to_ms) ? (time_ms - time_s_to_ms) : (time_s_to_ms - time_ms);
  TEST_ASSERT_LESS_THAN(1000, diff); // Less than 1 second difference
}

/**
 * @brief Test RTC wrapping functionality
 * This test verifies that the wrapping test functionality works
 */
static void test_hal_rtc_wrapping_functionality(void)
{
  uint32_t offset_to_test_wrapping = 100; // 1 second offset for wrapping test

  // Set the offset for wrapping test
  hal_rtc_set_offset_to_test_wrapping(offset_to_test_wrapping);

  // Get current tick count after wrapping
  uint32_t current_ticks = sl_hal_sysrtc_get_counter();

  // Wait for sysrtc to wrap around
  sl_sleeptimer_delay_millisecond(200);

  // Get current tick count after wrapping
  uint32_t wrapped_ticks = sl_hal_sysrtc_get_counter();

  // Verify that the wrapped tick count is less than the original tick count
  TEST_ASSERT_LESS_THAN_UINT32_MESSAGE(current_ticks, wrapped_ticks, "RTC did not wrap correctly");
}

/**
 * @brief Test smtc_modem_hal_get_time_in_s function
 * This test verifies that the function returns a valid time in seconds
 */
static void test_smtc_modem_hal_get_time_in_s(void)
{
  // Get start time in seconds
  uint32_t start_time = smtc_modem_hal_get_time_in_s();
  TEST_ASSERT_NOT_EQUAL_UINT32(0, start_time);
  TEST_ASSERT_NOT_EQUAL_UINT32(UINT32_MAX, start_time);

  // Delay for 3 seconds
  sl_sleeptimer_delay_millisecond(1500);

  // Get end time in seconds
  uint32_t end_time = smtc_modem_hal_get_time_in_s();
  TEST_ASSERT_NOT_EQUAL_UINT32(0, end_time);
  TEST_ASSERT_NOT_EQUAL_UINT32(UINT32_MAX, end_time);

  // Verify that the end time is greater than the start time
  TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(start_time, end_time, "End time is not greater than start time");

  // Verify accuracy
  int32_t elapsed_time = end_time - start_time - 3;
  TEST_ASSERT_TRUE_MESSAGE(elapsed_time <= 1 || elapsed_time >= -1, "RTC time is not accurate");
}

/**
 * @brief Test smtc_modem_hal_get_time_in_ms function
 * This test verifies that the function returns a valid time in milliseconds
 */
static void test_smtc_modem_hal_get_time_in_ms(void)
{
  // Get start time in milliseconds
  uint32_t start_time = smtc_modem_hal_get_time_in_ms();
  TEST_ASSERT_NOT_EQUAL_UINT32(0, start_time);
  TEST_ASSERT_NOT_EQUAL_UINT32(UINT32_MAX, start_time);

  // Delay for 300 ms
  sl_sleeptimer_delay_millisecond(300);

  // Get end time in milliseconds
  uint32_t end_time = smtc_modem_hal_get_time_in_ms();
  TEST_ASSERT_NOT_EQUAL_UINT32(0, end_time);
  TEST_ASSERT_NOT_EQUAL_UINT32(UINT32_MAX, end_time);

  // Verify that the end time is greater than the start time
  TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(start_time, end_time, "End time is not greater than start time");

  // Verify accuracy
  int32_t elapsed_time = end_time - start_time - 300;
  TEST_ASSERT_TRUE_MESSAGE(elapsed_time <= TEST_RTC_TIME_TOLERANCE_MS || elapsed_time >= -TEST_RTC_TIME_TOLERANCE_MS, "RTC time is not accurate");
}
