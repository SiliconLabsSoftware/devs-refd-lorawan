/***************************************************************************//**
 * @file smtc_hal_gpio.c
 * @brief The LBM GPIO Hardware Abstraction Layer Source File
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
#include "smtc_config.h"
#include "smtc_hal_gpio.h"
#include "em_cmu.h"
#include "sl_gpio.h"
#include "app_log.h"

static void gpio_callback_wrapper(uint8_t pin, void *context);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/*!
 * Array holding attached IRQ gpio data context
 * EFR32 devices have 16 interrupt channels (0-15), so we only need 16 entries
 */
static hal_gpio_irq_t const *gpio_irq[MAX_INTERRUPT_PIN];

// -----------------------------------------------------------------------------
//                                Static functions declaration
// -----------------------------------------------------------------------------

static void gpio_callback_wrapper(uint8_t pin, void *context)
{
  (void) pin;
  hal_gpio_irq_t *irq = (hal_gpio_irq_t*) context;

  if (irq != NULL && irq->callback != NULL) {
    irq->callback(irq->context);
  }
}

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* ------------ MCU input pin handling ------------*/

void hal_gpio_init_in(hal_gpio_irq_t *irq)
{
  if (irq == NULL) {
    app_log_debug("IRQ context is NULL");
    return;
  }
  if (irq->pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("Interrupt pin outside of 0-15 range: %d", irq->pin);
    return;
  }

  GPIO_Mode_TypeDef gpio_mode = gpioModeInput;
  uint32_t out_value = 0;

  // Configure GPIO pin mode based on pull_mode
  switch (irq->pull_mode) {
    case SMTC_HAL_GPIO_PULL_MODE_NONE:
      break;

    case SMTC_HAL_GPIO_PULL_MODE_UP:
      gpio_mode = gpioModeInputPull;
      out_value = 1; // Pull-up
      break;
    case SMTC_HAL_GPIO_PULL_MODE_DOWN:
      gpio_mode = gpioModeInputPull;
      out_value = 0; // Pull-down
      break;
    default:
      break;
  }

  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(irq->port, irq->pin, gpio_mode, out_value);

  // Configure interrupt directly if provided
  if (irq != NULL && irq->callback != NULL
      && irq->irq_mode != SL_GPIO_INTERRUPT_NO_EDGE) {
    GPIO_IntDisable(1U << irq->pin);

    // Configure external pin interrupt directly using port/pin
    sl_gpio_t int_config = { .port = irq->port, .pin = irq->pin };
    int32_t int_no = (int32_t) irq->pin;
    sl_status_t status = sl_gpio_configure_external_interrupt(&int_config,
                                                              &int_no, irq->irq_mode, gpio_callback_wrapper, // Use wrapper function directly
                                                              irq);

    if (status != SL_STATUS_OK) {
      app_log_debug("Fail to configure external pin: %d", irq->pin);
      return;
    }

    GPIO_IntEnable(1U << irq->pin);
  }
}

void hal_gpio_init_out(const GPIO_Port_TypeDef port, const uint32_t pin,
                       const uint32_t value)
{
  if (pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("GPIO init out: Pin %ld exceeds maximum 15", pin);
    return;
  }

  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(port, pin, gpioModePushPull, 0);

  (value != 0) ? GPIO_PinOutSet(port, pin) : GPIO_PinOutClear(port, pin);
}

void hal_gpio_irq_attach(hal_gpio_irq_t *irq)
{
  if ((irq != NULL) && (irq->callback != NULL)
      && (irq->pin < MAX_INTERRUPT_PIN)) {
    hal_gpio_init_in(irq);
  } else if (irq != NULL && irq->pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("IRQ attach: Pin %d exceeds maximum %d", irq->pin,
                  MAX_INTERRUPT_PIN);
  }
}

void hal_gpio_irq_detach(const hal_gpio_irq_t *irq)
{
  if (irq != NULL && irq->pin < MAX_INTERRUPT_PIN) {
    gpio_irq[irq->pin] = NULL;
  } else if (irq != NULL && irq->pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("IRQ detach: Pin %d exceeds maximum %d", irq->pin,
                  MAX_INTERRUPT_PIN - 1);
  }
}

void hal_gpio_irq_enable(void)
{
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void hal_gpio_irq_disable(void)
{
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
}

/* ------------ MCU pin state control ------------*/

void hal_gpio_set_value(const GPIO_Port_TypeDef port, const uint8_t pin,
                        const uint32_t value)
{
  if (pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("GPIO set value: Pin %d exceeds maximum 15", pin);
    return;
  }

  if (value == 0) {
    GPIO_PinOutClear(port, pin);
  } else {
    GPIO_PinOutSet(port, pin);
  }
}

uint32_t hal_gpio_get_value(const GPIO_Port_TypeDef port, const uint8_t pin)
{
  if (pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("GPIO get value: Pin %d exceeds maximum 15", pin);
    return 0;
  }

  return GPIO_PinInGet(port, pin);
}

void hal_gpio_clear_pending_irq(const uint8_t pin)
{
  if (pin >= MAX_INTERRUPT_PIN) {
    app_log_debug("GPIO clear IRQ: Pin %d exceeds maximum 15", pin);
    return;
  }

  GPIO_IntClear(1 << pin);
}

void hal_gpio_enable_clock(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
}
