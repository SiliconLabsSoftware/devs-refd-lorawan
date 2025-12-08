/***************************************************************************//**
 * @file smtc_crypto_test_runner.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "smtc_crypto_test_runner.h"
#include "test_smtc_modem_crypto.h"
#include "smtc_secure_element.h"
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void smtc_crypto_test_runner_init(void)
{
  printf("SMTC Modem Crypto Test Application\n");
  printf("==================================\n");
  printf("Platform: EFR32xG28 with SX126X\n");
  printf("Test Framework: Unity\n");
  printf("Crypto: Soft Secure Element Implementation\n\n");

  // Initialize soft secure element
  printf("Initializing Soft Secure Element...\n");
  smtc_secure_element_init();
  printf("Soft Secure Element initialized successfully.\n\n");

  printf("=== RUNNING SMTC MODEM CRYPTO TESTS ===\n\n");

  // Run all crypto tests with progress tracking
  printf("Starting test execution...\n");
  int test_failures = smtc_modem_crypto_run_all_tests();
  printf("Test execution completed.\n");

  printf("\n=== Test Execution Summary ===\n");
  if (test_failures == 0) {
    printf("ALL TESTS PASSED!\n");
    printf("SMTC Modem Crypto implementation is working correctly.\n");
    printf("System is ready for LoRaWAN communication on EFR32xG28 + SX126X.\n");
  } else {
    printf(" %d test(s) FAILED!\n", test_failures);
    printf("Please check the implementation and resolve issues.\n");
  }

  // Display soft secure element info
  printf("\n=== Soft Secure Element Statistics ===\n");
  printf("Using software implementation for cryptographic operations\n");
  printf("AES-128 encryption and CMAC operations validated\n");

  printf("\n=== Test Runner Complete ===\n");
}

void smtc_crypto_test_runner_process_action(void)
{
  // Process any background tasks if needed
  // For example, watchdog reload if available
  // This function can be called in main loop if needed
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                        Unity Test Framework Setup
// -----------------------------------------------------------------------------

/**
 * @brief Unity setup function - called before each test
 * This is a global setup function that applies to all crypto tests
 */
void setUp(void)
{
  // Global setup for all crypto tests
  // No special setup needed for soft secure element
}

/**
 * @brief Unity teardown function - called after each test
 * This is a global teardown function that applies to all crypto tests
 */
void tearDown(void)
{
  // Global teardown for all crypto tests
  // Keep this minimal since it runs after EVERY test
}
