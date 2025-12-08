/***************************************************************************//**
 * @brief Unit tests for LBM HAL Watchdog
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
#include "test_sl_lbm_hal_watchdog.h"
#include "unity.h"
#include "smtc_hal_watchdog.h"
#include "sl_hal_wdog.h"
#include "sl_device_peripheral.h"
#include "em_rmu.h"
#include "smtc_modem_hal.h"
#include "sl_sleeptimer.h"
#include "sl_clock_manager.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define DEFAULT_WDOG_CLOCK_PERIOD 32769u
#define TEST_TIMEOUT_TOLARANCE    10
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_hal_watchdog_initialization(void);
static void test_hal_watchdog_reload_functionality(void);
static void test_hal_watchdog_multiple_reloads(void);
static void test_smtc_modem_hal_watchdog_reload(void);
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static volatile uint32_t wdog_timeout_in_ms = 0;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_watchdog_test(void)
{
  UNITY_BEGIN();

  // Test 1: Initialize watchdog and verify successful initialization
  RUN_TEST(test_hal_watchdog_initialization);

  // Test 2: Test watchdog reload functionality
  RUN_TEST(test_hal_watchdog_reload_functionality);

  // Test 3: Test multiple consecutive reloads
  RUN_TEST(test_hal_watchdog_multiple_reloads);

  // Test 4: Test smtc_modem_hal_watchdog_reload
  RUN_TEST(test_smtc_modem_hal_watchdog_reload);

  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test watchdog initialization
 * This test verifies that the watchdog initializes without errors
 */
static void test_hal_watchdog_initialization(void)
{
  // Get reset reason
  uint32_t reset_cause = 0;
  reset_cause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // Initialize the watchdog
  hal_watchdog_init();

  // Verify watchdog is enabled
  bool is_enabled = sl_hal_wdog_is_enabled(WDOG0);
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled after initialization");

  // Calculate timeout in ms
  sl_clock_branch_t clock_branch = sl_device_peripheral_get_clock_branch(SL_PERIPHERAL_WDOG0);
  uint32_t freq = 0;
  sl_clock_manager_get_clock_branch_frequency(clock_branch, &freq);
  wdog_timeout_in_ms = (DEFAULT_WDOG_CLOCK_PERIOD / freq) * 1000;

  if (reset_cause & EMU_RSTCAUSE_WDOG0) {
    TEST_PASS_MESSAGE("WDOG successful reset after expired timeout");
  } else {
    // Delay until watchdog timeout
    sl_sleeptimer_delay_millisecond(wdog_timeout_in_ms + TEST_TIMEOUT_TOLARANCE);
  }
}

/**
 * @brief Test watchdog reload functionality
 * This test verifies that watchdog reload works correctly
 */
static void test_hal_watchdog_reload_functionality(void)
{
  // Check if watchdog is enabled
  bool is_enabled = sl_hal_wdog_is_enabled(WDOG0);

  // Assert that the watchdog is enabled before reload
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled before reload");

  // Test reload functionality
  hal_watchdog_reload();

  // Delay until watchdog counter appropriate timeout value
  sl_sleeptimer_delay_millisecond(wdog_timeout_in_ms - TEST_TIMEOUT_TOLARANCE);

  // Feed the watchdog to prevent reset
  hal_watchdog_reload();

  // Delay half of timeout to verify result
  sl_sleeptimer_delay_millisecond((wdog_timeout_in_ms / 2) - TEST_TIMEOUT_TOLARANCE);

  // This test case will pass if MCU would not reset.
  TEST_PASS();

  // Verify that the watchdog is still enabled after reload
  is_enabled = sl_hal_wdog_is_enabled(WDOG0);
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled after reload");

  // Feed the watchdog to prevent reset
  hal_watchdog_reload();
}

/**
 * @brief Test multiple consecutive watchdog reloads
 * This test verifies that multiple reloads work correctly in sequence
 */
static void test_hal_watchdog_multiple_reloads(void)
{
  // Check if watchdog is enabled
  bool is_enabled = sl_hal_wdog_is_enabled(WDOG0);

  // Assert that the watchdog is enabled before multiple reloads
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled before multiple reloads");

  // Test multiple reloads in sequence to ensure stability
  for (int i = 0; i < 5; i++) {
    hal_watchdog_reload();
  }

  // Verify that the watchdog is still enabled after multiple reloads
  is_enabled = sl_hal_wdog_is_enabled(WDOG0);

  // Assert that the watchdog is enabled before reload
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled before multiple reloads");
}

/**
 * @brief Test SMTC modem HAL watchdog reload functionality
 */
static void test_smtc_modem_hal_watchdog_reload(void)
{
  // Check if watchdog is enabled
  bool is_enabled = sl_hal_wdog_is_enabled(WDOG0);

  // Assert that the watchdog is enabled before reload
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled before reload");

  // Test reload functionality
  smtc_modem_hal_reload_wdog();

  // Delay until watchdog counter appropriate timeout value
  sl_sleeptimer_delay_millisecond(wdog_timeout_in_ms - TEST_TIMEOUT_TOLARANCE);

  // Feed the watchdog to prevent reset
  smtc_modem_hal_reload_wdog();

  // Delay half of timeout to verify result
  sl_sleeptimer_delay_millisecond((wdog_timeout_in_ms / 2) - TEST_TIMEOUT_TOLARANCE);

  // This test case will pass if MCU would not reset.
  TEST_PASS();

  // Verify that the watchdog is still enabled after reload
  is_enabled = sl_hal_wdog_is_enabled(WDOG0);
  TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Watchdog should be enabled after reload");

  // Feed the watchdog to prevent reset
  smtc_modem_hal_reload_wdog();
}
