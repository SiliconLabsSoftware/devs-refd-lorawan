/***************************************************************************//**
 * @file smtc_crypto_test_runner.h
 * @brief Standalone test runner for SMTC Modem Crypto unit tests
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
#ifndef SMTC_CRYPTO_TEST_RUNNER_H
#define SMTC_CRYPTO_TEST_RUNNER_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Initialize and run SMTC Modem Crypto unit tests
 *
 * This function initializes the mock secure element and executes all
 * SMTC Modem Crypto unit tests. It provides a standalone test runner
 * that can be used independently without the main HAL test runner.
 *
 * Features tested:
 * - Payload encryption/decryption for LoRaWAN frames
 * - MIC computation and verification for message integrity
 * - Join request/accept processing
 * - Session key derivation from join parameters
 * - Multicast group key management
 * - Class B ping slot randomization
 * - Service encryption for modem features
 * - Comprehensive error handling and parameter validation
 *
 * @return Number of failed tests (0 if all tests pass)
 */
void smtc_crypto_test_runner_init(void);

/**
 * @brief Process crypto test runner actions
 *
 * This function should be called periodically during test execution to
 * maintain system operation. Can be used for watchdog reload or other
 * system maintenance tasks.
 */
void smtc_crypto_test_runner_process_action(void);

#ifdef __cplusplus
}
#endif

#endif // SMTC_CRYPTO_TEST_RUNNER_H
