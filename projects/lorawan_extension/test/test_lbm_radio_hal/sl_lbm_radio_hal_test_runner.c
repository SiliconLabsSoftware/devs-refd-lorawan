/***************************************************************************//**
 * @file sl_lbm_radio_hal_test_runner.c
 * @brief Main test runner for LBM Radio HAL unit tests
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
#include "unity.h"
#include <stdio.h>
#include "test_sl_lbm_radio_hal.h"
#include "sl_lbm_radio_hal_test_runner.h"
#include "sl_spidrv_instances.h"
#include "sl_lbm_hal_spi.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_BANNER_WIDTH 60

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_radio_hal_setup_test_environment(void)
{
  printf("Radio HAL test environment initialized.\n");
  sl_lbm_hal_spi_set_radio_handle(sl_spidrv_eusart_exp_handle);
}

int sl_radio_hal_run_all_tests(void)
{
  // Print test banner
  printf("%.*s\n", TEST_BANNER_WIDTH, "============================================================");
  printf("Radio HAL Unity Test Runner\n");
  printf("Testing Global API Functions in sl_radio_hal.c\n");
  printf("%.*s\n", TEST_BANNER_WIDTH, "============================================================");

  UNITY_BEGIN();

  printf("\nRunning all test cases...\n");

  // sx126x_hal_write() tests
  RUN_TEST(test_sx126x_hal_write_ValidParams_ShouldReturnOK);
  RUN_TEST(test_sx126x_hal_write_ZeroCommandLength_ShouldReturnOK);
  RUN_TEST(test_sx126x_hal_write_SleepCommand_ShouldSetSleepMode);

  // sx126x_hal_read() tests
  RUN_TEST(test_sx126x_hal_read_ValidParams_ShouldReturnOK);
  RUN_TEST(test_sx126x_hal_read_ZeroDataLength_ShouldReturnOK);

  // sx126x_hal_reset() tests
  RUN_TEST(test_sx126x_hal_reset_ValidContext_ShouldReturnOK);

  // sx126x_hal_wakeup() tests
  RUN_TEST(test_sx126x_hal_wakeup_ValidContext_ShouldReturnOK);

  // Hardware tests for sx126x_hal_write()
  RUN_TEST(test_sx126x_hal_write_WithRealHardware_ShouldCommunicate);
  RUN_TEST(test_sx126x_hal_write_RealSleepCommand_ShouldPutRadioToSleep);

  // Hardware tests for sx126x_hal_read()
  RUN_TEST(test_sx126x_hal_read_WithRealHardware_ShouldReceiveData);
  RUN_TEST(test_sx126x_hal_read_ReadRadioVersion_ShouldReturnValidData);

  // Hardware tests for sx126x_hal_reset()
  RUN_TEST(test_sx126x_hal_reset_WithRealHardware_ShouldResetRadio);

  // Hardware tests for sx126x_hal_wakeup()
  RUN_TEST(test_sx126x_hal_wakeup_WithRealHardware_ShouldWakeRadio);

  int result = UNITY_END();

  // Print test summary
  printf("\n%.*s\n", TEST_BANNER_WIDTH, "============================================================");
  printf("Test Results Summary\n");
  printf("%.*s\n", TEST_BANNER_WIDTH, "============================================================");

  if (result == 0) {
    printf("ALL TESTS PASSED!\n");
    printf("Status: SUCCESS\n");
  } else {
    printf("SOME TESTS FAILED!\n");
    printf("Status: FAILURE\n");
  }

  printf("%.*s\n", TEST_BANNER_WIDTH, "============================================================");

  return result;
}

/* --- EOF ------------------------------------------------------------------ */
