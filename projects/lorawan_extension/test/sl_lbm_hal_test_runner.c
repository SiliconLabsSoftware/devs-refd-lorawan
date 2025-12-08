/***************************************************************************//**
 * @file sl_lbm_hal_test_runner.c
 * @brief Main test runner for LBM HAL unit tests
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
#include "sl_lbm_hal_test_runner.h"
#include <stdio.h>
#include "unity.h"
#include "test_sl_lbm_hal_watchdog/test_sl_lbm_hal_watchdog.h"
#include "test_sl_lbm_hal_rtc/test_sl_lbm_hal_rtc.h"
#include "test_sl_lbm_hal_lp_timer/test_sl_lbm_hal_lp_timer.h"
#include "test_sl_lbm_hal_rng/test_sl_lbm_hal_rng.h"
#include "test_sl_lbm_hal_context/test_sl_lbm_hal_context.h"
#include "test_sl_lbm_hal_gpio/test_sl_lbm_hal_gpio.h"
#include "test_sl_lbm_hal_crashlog/test_sl_lbm_hal_crashlog.h"
#include "test_sl_lbm_hal_iadc/test_sl_lbm_hal_iadc.h"
// Include HAL headers for global setup/teardown
#include "smtc_hal_watchdog.h"
#include "smtc_hal_rtc.h"
#include "smtc_hal_lp_timer.h"
#include "smtc_hal_rng.h"
#include "smtc_hal_gpio.h"
#include "smtc_hal_context.h"
#include "smtc_hal_crashlog.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**
 * @brief Run all LBM HAL unit tests
 *
 * This function executes all the unit tests for the LBM HAL modules including
 * watchdog, RTC, and LP timer. It uses the Unity test framework to perform the tests and
 * report results.
 *
 * @return Total number of failed tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_all_tests(void);

/**
 * @brief Run all LBM HAL watchdog tests
 *
 * @return Number of failed watchdog tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_watchdog_tests(void);

/**
 * @brief Run all LBM HAL RTC tests
 *
 * @return Number of failed RTC tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_rtc_tests(void);

/**
 * @brief Run all LBM HAL LP Timer tests
 *
 * @return Number of failed LP Timer tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_lp_timer_tests(void);

/**
 * @brief Run all LBM HAL RNG tests
 *
 * @return Number of failed RNG tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_rng_tests(void);

/**
 * @brief Run all LBM HAL Context Saving tests
 *
 * @return Number of failed Context Saving tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_context_tests(void);

/**
 * @brief Run all LBM HAL GPIO tests
 *
 * @return Number of failed GPIO tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_gpio_tests(void);

/**
 * @brief Run all LBM HAL Crashlog tests
 *
 * @return Number of failed Crashlog tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_crashlog_tests(void);

/**
 * @brief Run all LBM HAL IADC tests
 *
 * @return Number of failed IADC tests (0 if all tests pass)
 */
static int sli_lbm_hal_run_iadc_tests(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void sl_lbm_hal_test_runner_init(void)
{
  printf("LBM HAL Test Application\n");
  printf("========================\n");

  // You can control test execution via compile-time flags,
  // runtime configuration, or user input
  printf("\n=== RUNNING IN TEST MODE ===\n");

  // Run all LBM HAL tests
  int test_failures = sli_lbm_hal_run_all_tests();

  if (test_failures == 0) {
    printf("\nAll tests passed! System is ready for normal operation.\n");
  } else {
    printf("\n%d test(s) failed! Please check the implementation.\n", test_failures);
  }
}

void sl_lbm_hal_test_runner_process_action(void)
{
  // Process test actions
  // Feed wdog timer to keep MCU from soft-reset
  hal_watchdog_reload();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static int sli_lbm_hal_run_all_tests(void)
{
  int total_failures = 0;

  printf("\n=== Running LBM HAL Unit Tests ===\n");

  // Run watchdog tests
  printf("\n--- Running Watchdog Tests ---\n");
  total_failures += sli_lbm_hal_run_watchdog_tests();

  // Run RTC tests
  printf("\n--- Running RTC Tests ---\n");
  total_failures += sli_lbm_hal_run_rtc_tests();

  // Run LP timer tests
  printf("\n--- Running LP Timer Tests ---\n");
  total_failures += sli_lbm_hal_run_lp_timer_tests();

  // Run RNG tests
  printf("\n--- Running RNG Tests ---\n");
  total_failures += sli_lbm_hal_run_rng_tests();

  // Run Context saving tests
  printf("\n--- Running Context Saving Tests ---\n");
  total_failures += sli_lbm_hal_run_context_tests();

  // Run GPIO tests
  printf("\n--- Running GPIO Tests ---\n");
  total_failures += sli_lbm_hal_run_gpio_tests();

  // Run Crashlog tests
  printf("\n--- Running Crashlog Tests ---\n");
  total_failures += sli_lbm_hal_run_crashlog_tests();

  // Run IADC tests
  printf("\n--- Running IADC Tests ---\n");
  total_failures += sli_lbm_hal_run_iadc_tests();

  // Print summary
  printf("\n=== Test Summary ===\n");
  if (total_failures == 0) {
    printf("All LBM HAL tests PASSED!\n");
  } else {
    printf("LBM HAL tests completed with %d failures.\n", total_failures);
  }

  return total_failures;
}

static int sli_lbm_hal_run_watchdog_tests(void)
{
  return sl_lbm_hal_run_watchdog_test();
}

static int sli_lbm_hal_run_rtc_tests(void)
{
  return sl_lbm_hal_run_rtc_test();
}

static int sli_lbm_hal_run_lp_timer_tests(void)
{
  return sl_lbm_hal_run_lp_timer_test();
}

static int sli_lbm_hal_run_rng_tests(void)
{
  return sl_lbm_hal_run_rng_test();
}

static int sli_lbm_hal_run_context_tests(void)
{
  return sl_lbm_hal_run_context_test();
}

static int sli_lbm_hal_run_gpio_tests(void)
{
  return sl_lbm_hal_run_gpio_test();
}

static int sli_lbm_hal_run_crashlog_tests(void)
{
  return sl_lbm_hal_run_crashlog_test();
}

static int sli_lbm_hal_run_iadc_tests(void)
{
  return sl_lbm_hal_run_iadc_test();
}

// -----------------------------------------------------------------------------
//                        Unity Test Framework Setup
// -----------------------------------------------------------------------------

/**
 * @brief Unity setup function - called before each test
 * This is a global setup function that applies to all tests
 */
void setUp(void)
{
  // Global setup for all tests
  // Keep this minimal since it runs before EVERY test
}

/**
 * @brief Unity teardown function - called after each test
 * This is a global teardown function that applies to all tests
 */
void tearDown(void)
{
  // Global teardown for all tests
  // Keep MCU from soft-reset for others tests to run
  hal_watchdog_reload();
}
