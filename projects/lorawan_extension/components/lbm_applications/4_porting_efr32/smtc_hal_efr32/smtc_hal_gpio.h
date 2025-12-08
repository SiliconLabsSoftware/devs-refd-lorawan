/***************************************************************************//**
 * @file hal_gpio.h
 * @brief GPIO HAL Header File
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

#ifndef SMTC_HAL_GPIO_H
#define SMTC_HAL_GPIO_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "stdint.h"
#include "em_gpio.h"
#include "sl_gpio.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define MAX_INTERRUPT_PIN 15

/*!
 * GPIO Pull modes
 */
typedef enum gpio_pull_mode_e {
  SMTC_HAL_GPIO_PULL_MODE_NONE = 0,
  SMTC_HAL_GPIO_PULL_MODE_UP = 1,
  SMTC_HAL_GPIO_PULL_MODE_DOWN = 2,
} hal_gpio_pull_mode_t;

/*!
 * GPIO IRQ data context
 */
typedef struct gpio_irq_s {
  GPIO_Port_TypeDef port;
  uint8_t pin;
  hal_gpio_pull_mode_t pull_mode;
  sl_gpio_interrupt_flag_t irq_mode;
  void *context;
  void (*callback)(void *context);
} hal_gpio_irq_t;

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*****************************************************************************
 * @brief
 *   Initializes given pin as input.
 *
 * @param[in] irq_mode
 *       MCU IQR mode [BSP_GPIO_IRQ_MODE_OFF,
 *                 BSP_GPIO_IRQ_MODE_RISING,
 *                 BSP_GPIO_IRQ_MODE_FALLING,
 *                 BSP_GPIO_IRQ_MODE_RISING_FALLING].
 *
 * @param[in/out] irq
 *   Pointer to IRQ data context. NULL when BSP_GPIO_IRQ_MODE_OFF pin parameter is initialized.
 ******************************************************************************/
void hal_gpio_init_in(hal_gpio_irq_t *irq);

/***************************************************************************//**
 * @brief
 *   Initializes given pin as output with given initial value.
 *
 * @param[in] port
 *   MCU GPIO port.
 *
 * @param[in] pin
 *   MCU pin number to be initialize.
 *
 * @param[in] value
 *       MCU initial pin value.
 ******************************************************************************/
void hal_gpio_init_out(const GPIO_Port_TypeDef port, const uint32_t pin,
                       const uint32_t value);

/***************************************************************************//**
 * @brief
 *   Attaches given callback to the MCU IRQ handler.
 *
 * @param[in] irq
 *   Pointer to IRQ data context.
 ******************************************************************************/
void hal_gpio_irq_attach(hal_gpio_irq_t *irq);

/***************************************************************************//**
 * @brief
 *   Detaches callback from the MCU IRQ handler.
 *
 * @param[in] irq
 *   Pointer to IRQ data context.
 ******************************************************************************/
void hal_gpio_irq_detach(const hal_gpio_irq_t *irq);

/***************************************************************************//**
 * @brief
 *   Enables all GPIO MCU interrupts.
 ******************************************************************************/
void hal_gpio_irq_enable(void);

/***************************************************************************//**
 * @brief
 *   Disables all GPIO MCU interrupts.
 ******************************************************************************/
void hal_gpio_irq_disable(void);

/***************************************************************************//**
 * @brief
 *   Initializes given pin as output with given initial value.
 *
 * @param[in] port
 *   MCU GPIO port.
 *
 * @param[in] pin
 *   MCU pin number to be initialize.
 *
 * @param[in] value
 *       MCU pin state to be set.
 ******************************************************************************/
void hal_gpio_set_value(const GPIO_Port_TypeDef port, const uint8_t pin,
                        const uint32_t value);

/***************************************************************************//**
 * @brief
 *  Gets MCU pin state value.
 *
 * @param[in] port
 *   MCU GPIO port.
 *
 * @param[in] pin
 *   MCU pin to be read.
 *
 * @return
 *   Current MCU pin state.
 ******************************************************************************/
uint32_t hal_gpio_get_value(const GPIO_Port_TypeDef port, const uint8_t pin);

/***************************************************************************//**
 * @brief
 *   Clears a pending irq on a pin.
 *
 * @param[in] pin
 *   Pin for which pending state is to be cleared.
 ******************************************************************************/
void hal_gpio_clear_pending_irq(const uint8_t pin);

/***************************************************************************//**
 * @brief
 *   Enable gpio clock.
 ******************************************************************************/
void hal_gpio_enable_clock(void);

#endif /* SMTC_HAL_GPIO_H */
