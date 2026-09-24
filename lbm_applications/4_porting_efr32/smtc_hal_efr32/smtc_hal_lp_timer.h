/***************************************************************************//**
 * @file hal_lp_timer.h
 * @brief The LBM HAL Low Power Timer Header File
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
#ifndef SMTC_HAL_LP_TIMER_H
#define SMTC_HAL_LP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include "sl_status.h"
#include "sl_hal_letimer.h"
#include "sl_clock_manager.h"
#include "sl_device_peripheral.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef enum hal_lp_timer_id_e{
  SMTC_HAL_LP_TIMER_ID_1 = 0,
  SMTC_HAL_LP_TIMER_ID_2,
  SMTC_HAL_LP_TIMER_ID_3,
  SMTC_HAL_LP_TIMER_ID_4,
  SMTC_HAL_LP_TIMER_ID_5,
  SMTC_HAL_LP_TIMER_ID_6,
  SMTC_HAL_LP_TIMER_ID_7,
  SMTC_HAL_LP_TIMER_ID_8,
  SMTC_HAL_LP_TIMER_ID_END
} hal_lp_timer_id_t;

/**
 * @brief Timer IRQ handling data context
 */
typedef struct hal_lp_timer_irq_s{
  void* context;
  void  (*callback)(void* context);
} hal_lp_timer_irq_t;

/**
 * @brief Timer initialization context
 * example of initialization:
 *  const hal_lp_timer_init_t inst1 = {
 *      .id = SMTC_HAL_LP_TIMER_ID_1,
 *      .bus_clock = SL_BUS_CLOCK_LETIMER0,
 *      .enable_top = true,
 *  };
 */
typedef struct hal_lp_timer_init_s{
  hal_lp_timer_id_t id;
  sl_bus_clock_t bus_clock;
  sl_hal_letimer_repeat_mode_t repeat_mode;
  sl_peripheral_t peripheral;
  bool enable_top;
  uint32_t irq_num;
} hal_lp_timer_init_t;

#define SMTC_HAL_LP_TIMER_INIT_DEFAULT                 \
  {                                                    \
    .id = SMTC_HAL_LP_TIMER_ID_1,                      \
    .bus_clock = SL_BUS_CLOCK_LETIMER0,                \
    .repeat_mode = SL_HAL_LETIMER_REPEAT_MODE_ONESHOT, \
    .peripheral = SL_PERIPHERAL_LETIMER0,              \
    .enable_top = false,                               \
    .irq_num = LETIMER0_IRQn                           \
  };

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Initializes the Low Power Timer peripheral
 *
 *
 * @retval SL_STATUS_OK on success
 * @retval SL_STATUS_INVALID_INDEX if the id is invalid
 * @retval SL_STATUS_NOT_AVAILABLE if the LETIMER peripheral is not available
 * @retval SL_STATUS_FAIL if the LETIMER peripheral clock could not be enabled
 */
sl_status_t hal_lp_timer_init(void);

/**
 * @brief Starts the provided timer object for the given time
 *
 * @param [in] id Low power timer id
 * @param [in] milliseconds Number of milliseconds
 * @param [in] tmr_irq Timer IRQ handling data context
 * @retval SL_STATUS_OK on success
 * @retval SL_STATUS_FAIL if the timer could not be started
 */
void hal_lp_timer_start(const uint32_t milliseconds,
                        const hal_lp_timer_irq_t* tmr_irq);

/**
 * @brief Starts the provided timer object for the given time
 *
 */
void hal_lp_timer_stop(void);

/**
 * @brief Enables timer interrupts (HW timer only)
 *
 * @retval SL_STATUS_OK on success
 * @retval SL_STATUS_FAIL if the timer irq could not be enabled
 */
void hal_lp_timer_irq_enable(void);

/**
 * @brief Disables timer interrupts (HW timer only)
 * @retval SL_STATUS_OK on success
 * @retval SL_STATUS_FAIL if the timer irq could not be disabled
 */
void hal_lp_timer_irq_disable(void);

/**
 * @brief Get the frequency of the low power timer peripheral
 */
uint32_t hal_lp_timer_get_peripheral_freq(void);
#ifdef __cplusplus
}
#endif
#endif // SMTC_HAL_LP_TIMER_H
