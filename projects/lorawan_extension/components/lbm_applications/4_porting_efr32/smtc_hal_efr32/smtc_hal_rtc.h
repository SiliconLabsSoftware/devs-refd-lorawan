/***************************************************************************//**
 * @file hal_rtc.h
 * @brief The LBM HAL RTC Header File
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
#ifndef SMTC_HAL_RTC_H
#define SMTC_HAL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define SMTC_HAL_RTC_EVENT_OVF (0x01)
#define SMTC_HAL_RTC_EVENT_COMP (0x02)
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**
 * @brief Initialize the RTC peripheral.
 */
void hal_rtc_init(void);

/**
 * @brief Get the current RTC time in seconds
 *
 * @remark Used for scheduling autonomous retransmissions (i.e: NbTrans),
 *         transmitting MAC answers, basically any delay without accurate time
 *         constraints. It is also used to measure the time spent inside the
 *         LoRaWAN process for the integrated failsafe.
 *
 * @retval rtc_time_s Current RTC time in seconds
 */
uint32_t hal_rtc_get_time_in_s(void);

/**
 * @brief Returns the current RTC time in milliseconds
 *
 * @remark Used to timestamp radio events (i.e: end of TX), will also be used
 * for ClassB
 *
 * @retval rtc_time_ms Current RTC time in milliseconds wraps every 49 days
 */
uint32_t hal_rtc_get_time_in_ms(void);

/**
 * @brief hal to test wrapping
 */
void hal_rtc_set_offset_to_test_wrapping(uint32_t offset_to_test_wrapping);
#ifdef __cplusplus
}
#endif
#endif // SMTC_HAL_RTC_H
