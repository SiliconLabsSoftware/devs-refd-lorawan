/***************************************************************************//**
 * @file test_sl_lbm_radio_hal_lp_timer.h
 * @brief Unit tests for LBM Radio HAL Low Power Timer module
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Labs Inc.
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

#ifndef TEST_SL_RADIO_HAL_H
#define TEST_SL_RADIO_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "unity.h"
#include "sx126x_hal.h"

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Test setup function - called before each test
 */
void setUp(void);

/**
 * @brief Test teardown function - called after each test
 */
void tearDown(void);

/**
 * @brief Test sx126x_hal_write() function
 */
void test_sx126x_hal_write_ValidParams_ShouldReturnOK(void);
void test_sx126x_hal_write_ZeroCommandLength_ShouldReturnOK(void);
void test_sx126x_hal_write_SleepCommand_ShouldSetSleepMode(void);

/**
 * @brief Test sx126x_hal_read() function
 */
void test_sx126x_hal_read_ValidParams_ShouldReturnOK(void);
void test_sx126x_hal_read_ZeroDataLength_ShouldReturnOK(void);

/**
 * @brief Test sx126x_hal_reset() function
 */
void test_sx126x_hal_reset_ValidContext_ShouldReturnOK(void);

/**
 * @brief Test sx126x_hal_wakeup() function
 */
void test_sx126x_hal_wakeup_ValidContext_ShouldReturnOK(void);

/**
 * @brief Hardware tests for sx126x_hal_write() function
 */
void test_sx126x_hal_write_WithRealHardware_ShouldCommunicate(void);
void test_sx126x_hal_write_RealSleepCommand_ShouldPutRadioToSleep(void);

/**
 * @brief Hardware tests for sx126x_hal_read() function
 */
void test_sx126x_hal_read_WithRealHardware_ShouldReceiveData(void);
void test_sx126x_hal_read_ReadRadioVersion_ShouldReturnValidData(void);

/**
 * @brief Hardware tests for sx126x_hal_reset() function
 */
void test_sx126x_hal_reset_WithRealHardware_ShouldResetRadio(void);

/**
 * @brief Hardware tests for sx126x_hal_wakeup() function
 */
void test_sx126x_hal_wakeup_WithRealHardware_ShouldWakeRadio(void);

#ifdef __cplusplus
}
#endif

#endif  // TEST_SL_RADIO_HAL_H

/* --- EOF ------------------------------------------------------------------ */
