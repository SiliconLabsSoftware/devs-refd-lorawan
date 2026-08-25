/***************************************************************************//**
 * @file hal_iadc.h
 * @brief LBM IADC Header File
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

#ifndef SMTC_HAL_IADC_H
#define SMTC_HAL_IADC_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "em_iadc.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define IADC_SLEEP_INTERVAL_MS            3

// Battery type selection
#define COIN_CELL                         0
#define BATTERY_TYPE                      COIN_CELL

#if (BATTERY_TYPE == COIN_CELL)
#define BATTERY_MIN_MV          2200
#define BATTERY_MAX_MV          3100
#define EXTERNAL_POWER_MV       3200
#endif

// Battery level return values
#define SMTC_HAL_BATTERY_ERROR            255
#define SMTC_HAL_BATTERY_FULL             254
#define SMTC_HAL_BATTERY_CRITICAL         1
#define SMTC_HAL_EXTERNAL_POWER_SUPPLY    0

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief
 *   Initializes IADC.
 *
 * @param[in] posInput
 *   Port/pin input for the positive side of the ADC.
 *
 * @param[in] negInput
 *   Port/pin input for the negative side of the ADC.
 ******************************************************************************/
void hal_init_iadc0(IADC_PosInput_t posInput, IADC_NegInput_t negInput);

/***************************************************************************//**
 * @brief
 *   Get battery level.
 *
 * @return
 *   0: The end-device is connected to an external power source.
 *   1..254: Battery level, where 1 is the minimum and 254 is the maximum.
 *   255: The end-device was not able to measure the battery level
 ******************************************************************************/
uint8_t hal_get_battery_level(void);

#endif  // SMTC_HAL_IADC_H
