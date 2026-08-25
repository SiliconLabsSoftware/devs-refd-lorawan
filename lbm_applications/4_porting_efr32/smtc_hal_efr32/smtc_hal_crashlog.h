/***************************************************************************//**
 * @file hal_crashlog.h
 * @brief Crashlog HAL Header File
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

#ifndef SMTC_HAL_CRASHLOG_H
#define SMTC_HAL_CRASHLOG_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief
 *   Stores the crashlog.
 *
 * @param[in] crash_string
 *   Crashlog string to be stored.
 *
 * @param[in] crash_string_length
 *   Crashlog string length.
 ******************************************************************************/

void hal_crashlog_store(const uint8_t *crash_string,
                        uint8_t crash_string_length);

/***************************************************************************//**
 * @brief
 *   Restores the crashlog.
 *
 * @param[out] crash_string
 *   Crashlog string to be restored.
 *
 * @param[out] crash_string_length
 *   Crashlog string length.
 ******************************************************************************/

void hal_crashlog_restore(uint8_t *crash_string,
                          uint8_t *crash_string_length);

/***************************************************************************//**
 * @brief
 *   Stores the crashlog status.
 *
 * @param[in] available
 *   True if a crashlog is available, false otherwise.
 ******************************************************************************/

void hal_crashlog_set_status(bool available);

/***************************************************************************//**
 * @brief
 *   Get the crashlog status.
 *
 * @return
 *   True if a crashlog is available, false otherwise.
 ******************************************************************************/
bool hal_crashlog_get_status(void);

/***************************************************************************//**
 * @brief
 *   Clear the crashlog.
 ******************************************************************************/
void hal_crashlog_clear();
#endif /* SMTC_HAL_CRASHLOG_H */
