/***************************************************************************//**
 * @file main_test_example.c
 * @brief Example of how to integrate LBM HAL unit tests into a main application
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
#include <stdio.h>
#include <stdbool.h>
#include "sl_lbm_hal_test_runner.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void run_test_mode(void);
static void run_normal_mode(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Example main function showing how to integrate unit tests
 */
int main(void)
{
  printf("LBM HAL Test Application\n");
  printf("========================\n");

  // You can control test execution via compile-time flags,
  // runtime configuration, or user input
#ifdef RUN_UNIT_TESTS
  run_test_mode();
#else
  // Check for runtime test trigger (e.g., button press, command line argument)
  bool run_tests = false;  // This could be set by button press, UART command, etc.

  if (run_tests) {
    run_test_mode();
  } else {
    run_normal_mode();
  }
#endif

  return 0;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Run the application in test mode
 */
static void run_test_mode(void)
{
  printf("\n=== RUNNING IN TEST MODE ===\n");

  // Run all LBM HAL tests
  int test_failures = sl_lbm_hal_run_all_tests();

  if (test_failures == 0) {
    printf("\n✅ All tests passed! System is ready for normal operation.\n");
  } else {
    printf("\n❌ %d test(s) failed! Please check the implementation.\n", test_failures);
  }

  // You can decide what to do after tests:
  // - Continue to normal mode if tests pass
  // - Halt execution if tests fail
  // - Provide diagnostics information

  if (test_failures == 0) {
    printf("\nSwitching to normal operation mode...\n");
    run_normal_mode();
  } else {
    printf("\nHalting due to test failures.\n");
    while (1) {
      // Halt or provide debug information
    }
  }
}

/**
 * @brief Run the application in normal mode
 */
static void run_normal_mode(void)
{
  printf("\n=== RUNNING IN NORMAL MODE ===\n");

  // Initialize your normal application components
  // sl_lbm_hal_watchdog_init();
  // sl_lbm_hal_rtc_init();
  // sl_lbm_hal_lp_timer_init(SL_LBM_HAL_LP_TIMER_ID_1);

  printf("LBM HAL application is running...\n");

  // Your normal application loop
  while (1) {
    // Normal application logic
    // sl_lbm_hal_watchdog_reload();
    // Handle LoRaWAN operations
    // etc.
  }
}
