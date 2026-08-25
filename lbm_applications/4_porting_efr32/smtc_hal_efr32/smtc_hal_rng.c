/***************************************************************************//**
 * @file hal_rng.c
 * @brief The LBM RNG Hardware Abstraction Layer Source File
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
#include "psa/crypto_types.h"
#include "psa/crypto_values.h"
#include "string.h"
#include "app_log.h"
#include "psa/crypto.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define RANDOM_BUFF_SIZE 4 /**< Random numbers buffer size. */

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

void hal_rng_init(void)
{
  // Initialize PSA Crypto
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    app_log_debug("PSA crypto init failed: %ld", status);
  }
}

uint32_t hal_rng_get_random(void)
{
  uint32_t rand_nb = 0;
  psa_status_t psa_status = psa_generate_random((uint8_t*) &rand_nb, sizeof(rand_nb));

  if (psa_status != PSA_SUCCESS) {
    return 0;
  }

  return rand_nb;
}

uint32_t hal_rng_get_random_in_range(const uint32_t val_1,
                                     const uint32_t val_2)
{
  if (val_1 <= val_2) {
    return hal_rng_get_random() % (val_2 - val_1 + 1) + val_1;
  } else {
    return hal_rng_get_random() % (val_1 - val_2 + 1) + val_2;
  }
}
