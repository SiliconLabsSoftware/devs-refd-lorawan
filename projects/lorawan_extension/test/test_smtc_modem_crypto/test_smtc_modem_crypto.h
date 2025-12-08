/***************************************************************************//**
 * @file test_smtc_modem_crypto.h
 * @brief Unit tests for SMTC Modem Crypto functions
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
#ifndef TEST_SMTC_MODEM_CRYPTO_H
#define TEST_SMTC_MODEM_CRYPTO_H

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
 * @brief Run all SMTC Modem Crypto unit tests
 *
 * This function executes all the unit tests for the SMTC Modem Crypto module.
 * It uses the Unity test framework to perform the tests and report results.
 * Tests include payload encryption/decryption, MIC computation/verification,
 * join processing, key derivation, and service encryption.
 *
 * @return Number of failed tests (0 if all tests pass)
 */
int smtc_modem_crypto_run_all_tests(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_SMTC_MODEM_CRYPTO_H
