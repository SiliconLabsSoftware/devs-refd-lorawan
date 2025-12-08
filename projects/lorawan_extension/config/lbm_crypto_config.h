/***************************************************************************//**
 * @file lbm_crypto_config.h
 * @brief LoRa Basic Modem Crypto Configuration
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

#ifndef LBM_CRYPTO_CONFIG_H
#define LBM_CRYPTO_CONFIG_H

// -----------------------------------------------------------------------------
//                             Crypto Implementation Selection
// -----------------------------------------------------------------------------

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

// Determine which crypto implementation to use
#ifdef SL_CATALOG_LBM_HW_CRYPTO_PRESENT
  // Hardware crypto (EFR32 Secure Element) is installed
  #define LBM_USE_HW_CRYPTO                1
  #define LBM_USE_SOFT_SE                  0
  #define LBM_CRYPTO_IMPLEMENTATION        "EFR32 Hardware Secure Element"
#else
  // Default to software secure element
  #define LBM_USE_HW_CRYPTO                0
  #define LBM_USE_SOFT_SE                  1
  #define LBM_CRYPTO_IMPLEMENTATION        "Software Secure Element"
#endif

// Common secure element header (used by both implementations)
#include "smtc_secure_element.h"

// -----------------------------------------------------------------------------
//                             Configuration Macros
// -----------------------------------------------------------------------------

// Debug and logging configuration
#ifndef LBM_CRYPTO_DEBUG_ENABLED
#define LBM_CRYPTO_DEBUG_ENABLED           0
#endif

// Performance monitoring
#ifndef LBM_CRYPTO_PERFORMANCE_MONITORING
#define LBM_CRYPTO_PERFORMANCE_MONITORING 0
#endif

// Error handling configuration
#ifndef LBM_CRYPTO_STRICT_ERROR_CHECKING
#define LBM_CRYPTO_STRICT_ERROR_CHECKING   1
#endif

// -----------------------------------------------------------------------------
//                             Informational Macros
// -----------------------------------------------------------------------------

// Crypto capabilities based on implementation
#ifdef SL_CATALOG_LBM_HW_CRYPTO_PRESENT
  #define LBM_CRYPTO_HAS_HARDWARE_AES      1
  #define LBM_CRYPTO_HAS_HARDWARE_CMAC     1
  #define LBM_CRYPTO_HAS_TRUE_RNG          1
  #define LBM_CRYPTO_HAS_KEY_STORAGE       1
#else
  #define LBM_CRYPTO_HAS_HARDWARE_AES      0
  #define LBM_CRYPTO_HAS_HARDWARE_CMAC     0
  #define LBM_CRYPTO_HAS_TRUE_RNG          0
  #define LBM_CRYPTO_HAS_KEY_STORAGE       0
#endif

// -----------------------------------------------------------------------------
//                             Compile-time Information
// -----------------------------------------------------------------------------

// Generate compile-time string for debugging
#ifdef SL_CATALOG_LBM_HW_CRYPTO_PRESENT
  #define LBM_CRYPTO_BUILD_INFO "LoRa Basic Modem with EFR32 Hardware Crypto"
#else
  #define LBM_CRYPTO_BUILD_INFO "LoRa Basic Modem with Software Crypto"
#endif

#endif // LBM_CRYPTO_CONFIG_H
