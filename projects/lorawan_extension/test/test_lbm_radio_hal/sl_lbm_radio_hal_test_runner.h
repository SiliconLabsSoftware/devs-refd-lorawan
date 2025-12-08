/***************************************************************************//**
 * @file sl_lbm_radio_hal_test_runner.h
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

#ifndef SL_RADIO_HAL_TEST_RUNNER_H
#define SL_RADIO_HAL_TEST_RUNNER_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**
 * @brief Setup test environment for Radio HAL tests
 */
void sl_radio_hal_setup_test_environment(void);

/**
 * @brief Run all Unity-based Radio HAL tests
 * @return 0 if all tests pass, non-zero if any test fails
 */
int sl_radio_hal_run_all_tests(void);

#ifdef __cplusplus
}
#endif

#endif  // SL_RADIO_HAL_TEST_RUNNER_H

/* --- EOF ------------------------------------------------------------------ */
