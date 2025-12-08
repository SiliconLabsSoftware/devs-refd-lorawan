/***************************************************************************//**
 * @file smtc_hal_watchdog.c
 * @brief The LBM HAL Watchdog Source File
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
#include "smtc_hal_watchdog.h"
#include "sl_hal_wdog.h"
#include "sl_clock_manager_tree_config.h"
#include "sl_clock_manager.h"
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

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

void hal_watchdog_init(void)
{
  // Initialize the watchdog with default configuration
  sl_hal_wdog_init_t wdogInit = SL_HAL_WDOG_INIT_DEFAULT;

#if SL_CLOCK_MANAGER_WDOG0CLK_SOURCE == CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO
  // Watchdog feed period max 32s with 1KHz clock source
  wdogInit.period_select = SL_WDOG_PERIOD_32k;
#else
  // Watchdog feed period max 8s with 32.768KHz default clock source
#endif
  // Enable clock for watchdog
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_WDOG0);

  // Initialize WDOG with our configuration
  sl_hal_wdog_init(WDOG0, &wdogInit);

  // Enable the watchdog - this start the watchdog timer
  sl_hal_wdog_enable(WDOG0);

  // Verify WDOG is enabled (optional)
  if (sl_hal_wdog_is_enabled(WDOG0)) {
    // Watchdog is now running

    // Feed the watchdog initially
    sl_hal_wdog_feed(WDOG0);

    // Wait for feed operation to complete
    sl_hal_wdog_wait_sync(WDOG0);
  }
}

void hal_watchdog_reload(void)
{
  // Feed the watchdog to prevent reset
  sl_hal_wdog_feed(WDOG0);

  // Wait for feed operation to complete
  sl_hal_wdog_wait_sync(WDOG0);
}
