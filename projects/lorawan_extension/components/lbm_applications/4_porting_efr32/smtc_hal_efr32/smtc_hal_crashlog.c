/***************************************************************************//**
 * @file hal_crashlog.c
 * @brief The LBM Crashlog Hardware Abstraction Layer Source File
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
#include "smtc_hal_crashlog.h"
#include "smtc_modem_hal.h"
#include "string.h"
#include "app_log.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#ifndef MIN
#define MIN(a, b) ( ( (a) < (b) ) ? (a) : (b) )
#endif

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

__attribute__( (section(".noinit") ))    static uint8_t crashlog_buff_noinit[CRASH_LOG_SIZE];
__attribute__( (section(".noinit") ))    static volatile uint8_t crashlog_length_noinit;
__attribute__( (section(".noinit") ) ) static volatile bool crashlog_available_noinit;

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void hal_crashlog_store(const uint8_t *crash_string,
                        uint8_t crash_string_length)
{
  // Clear previous crashlog
  memset(crashlog_buff_noinit, 0, CRASH_LOG_SIZE);

  crashlog_length_noinit = MIN(crash_string_length, CRASH_LOG_SIZE);
  memcpy(crashlog_buff_noinit, crash_string, crashlog_length_noinit);
  crashlog_available_noinit = true;

  app_log_debug("Crash log stored: %.*s\n", crashlog_length_noinit,
                crash_string);
}

void hal_crashlog_restore(uint8_t *crash_string,
                          uint8_t *crash_string_length)
{
  *crash_string_length =
    (crashlog_length_noinit > CRASH_LOG_SIZE)
    ? CRASH_LOG_SIZE : crashlog_length_noinit;
  memcpy(crash_string, crashlog_buff_noinit, *crash_string_length);
}

void hal_crashlog_set_status(bool available)
{
  crashlog_available_noinit = available;
}

bool hal_crashlog_get_status()
{
  return crashlog_available_noinit;
}

void hal_crashlog_clear()
{
  memset(crashlog_buff_noinit, 0, CRASH_LOG_SIZE);
  memset(&crashlog_length_noinit, 0, sizeof(crashlog_length_noinit));
  memset(&crashlog_available_noinit, 0, sizeof(crashlog_available_noinit));
}
