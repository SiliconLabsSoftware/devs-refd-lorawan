/***************************************************************************//**
 * @file app.c
 * @brief Application logic for SMTC Modem Crypto test example
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
#include "app.h"
#include "smtc_crypto_test_runner.h"
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
static bool tests_completed = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void app_init(void)
{
  printf("Initializing SMTC Modem Crypto Test Application...\n");

  // Initialize and run all crypto tests
  smtc_crypto_test_runner_init();

  tests_completed = true;
}

void app_process_action(void)
{
  if (tests_completed) {
    // Call the test runner process action (for any maintenance tasks)
    smtc_crypto_test_runner_process_action();

    // Add a small delay to prevent flooding output
    // In a real application, this could handle other background tasks
    static uint32_t counter = 0;
    counter++;

    if (counter % 1000000 == 0) {
      // Optional: Print periodic status every ~1M cycles
      // printf("System running... Tests completed successfully.\n");
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
