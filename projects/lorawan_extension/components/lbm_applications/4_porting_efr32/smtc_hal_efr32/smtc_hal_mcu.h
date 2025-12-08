/***************************************************************************//**
 * @file smtc_hal_mcu.h
 * @brief The HAL MCU Header File
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

#ifndef SMTC_HAL_MCU_H
#define SMTC_HAL_MCU_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "app_log.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/**
 * @brief Returns the minimum value between a and b
 *
 * @param [in] a 1st value
 * @param [in] b 2nd value
 * @retval Minimum value
 */
#ifndef MIN
#define MIN(a, b) ( ( (a) < (b) ) ? (a) : (b) )
#endif

#define mcu_panic(...)                         \
  do                                           \
  {                                            \
    app_log_error("mcu_panic:%s\n", __func__); \
    app_log_error("-> "__VA_ARGS__);           \
    smtc_modem_hal_reset_mcu( );               \
  } while ( 0 )
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Disable interrupts, begins critical section
 *
 * @param [in] mask Pointer to a variable where to store the CPU IRQ mask
 */
void hal_mcu_critical_section_begin(uint32_t* mask);

/**
 * @brief Ends critical section
 *
 * @param [in] mask Pointer to a variable where the CPU IRQ mask was stored
 */
void hal_mcu_critical_section_end(const uint32_t* mask);

/**
 * @brief Disable all irq at mcu side
 */
void hal_mcu_disable_irq(void);

/**
 * @brief Enable all irq at mcu side
 */
void hal_mcu_enable_irq(void);

/**
 * @brief Enable all irq at mcu side
 */
void hal_mcu_enable_irq(void);

/**
 * @brief Initialize the MCU
 */
void hal_mcu_init(void);

/**
 * @brief Reset mcu
 */
void hal_mcu_reset(void);

/**
 * @brief Sets the MCU in sleep mode for the given number of milliseconds.
 *
 * @param [in] milliseconds Number of milliseconds to stay in sleep mode
 */
void hal_mcu_set_sleep_for_ms(const int32_t milliseconds);

/**
 * @brief MCU wait for milliseconds
 *
 * @param [in] milliseconds Number of milliseconds to wait
 */
void hal_mcu_wait_ms(uint16_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif // SMTC_HAL_MCU_H

/* --- EOF ------------------------------------------------------------------ */
