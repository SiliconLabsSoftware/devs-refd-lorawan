/***************************************************************************//**
 * @file app.c
 * @brief Top level application functions
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
#include "sl_lbm_radio_hal_test_runner.h"
#include <stdio.h>

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  printf("\n==================================================\n");
  printf("Radio HAL Unit Test Runner\n");
  printf("==================================================\n");

  sl_radio_hal_setup_test_environment();

  printf("\nRunning all test cases...\n");
  int test_results = sl_radio_hal_run_all_tests();

  printf("\n==================================================\n");
  printf("Test Results Summary\n");
  printf("==================================================\n");

  if (test_results == 0) {
    printf("ALL TESTS PASSED!\n");
    printf("Status: SUCCESS\n");
  } else {
    printf("SOME TESTS FAILED!\n");
    printf("Failed tests: %d\n", test_results);
    printf("Status: FAILURE\n");
  }

  printf("==================================================\n\n");
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
}
