/***************************************************************************//**
 * @file hal_lp_timer.c
 * @brief The LBM HAL Low Power Timer Source File
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
#include "smtc_hal_lp_timer.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#else // SL_CATALOG_KERNEL_PRESENT
#include "em_device.h"
#include "sl_interrupt_manager.h"
#endif // SL_CATALOG_KERNEL_PRESENT

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#if !defined(SL_CATALOG_KERNEL_PRESENT)
#define LETIMER_IRQ_HANDLER LETIMER0_IRQHandler
#define SL_LETIMER_MAX_COUNTER 0xFFFFFF
#endif // SL_CATALOG_KERNEL_PRESENT
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
#if defined(SL_CATALOG_KERNEL_PRESENT)
static void hal_lp_timer_callback(void *argument);
#endif
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#if defined(SL_CATALOG_KERNEL_PRESENT)
// CMSIS2 timer handle
static osTimerId_t hal_lp_timer_id = NULL;
static bool callback_enable;
static bool callback_pending;
#else // SL_CATALOG_KERNEL_PRESENT
// Default initialization data for the low power timer
const hal_lp_timer_init_t hal_lp_timer = SMTC_HAL_LP_TIMER_INIT_DEFAULT;
// Frequency and ticks per millisecond for the low power timer
static volatile uint32_t hal_lp_timer_freq = 0;
// Ticks per millisecond for the low power timer
volatile float hal_lp_timer_ticks_per_ms = 0;
#endif // SL_CATALOG_KERNEL_PRESENT
// Timer callback context
static hal_lp_timer_irq_t hal_lp_timer_irq = {
  .context = NULL,
  .callback = NULL
};

#if defined(SL_CATALOG_KERNEL_PRESENT)
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
sl_status_t hal_lp_timer_init(void)
{
  // Create CMSIS2 timer (one-shot timer)
  hal_lp_timer_id = osTimerNew(hal_lp_timer_callback, osTimerOnce, NULL, NULL);

  if (hal_lp_timer_id == NULL) {
    return SL_STATUS_FAIL;
  }
  callback_enable = true;
  callback_pending = false;

  return SL_STATUS_OK;
}

void hal_lp_timer_start(const uint32_t milliseconds,
                        const hal_lp_timer_irq_t* tmr_irq)
{
  // Store the IRQ context and callback
  hal_lp_timer_irq = *tmr_irq;

  // Stop timer if already running
  if (hal_lp_timer_id != NULL) {
    osTimerStop(hal_lp_timer_id);
  }

  // Start the CMSIS2 timer with specified timeout in milliseconds
  osTimerStart(hal_lp_timer_id, milliseconds);
}

void hal_lp_timer_stop(void)
{
  // Stop the CMSIS2 timer
  if (hal_lp_timer_id != NULL) {
    osTimerStop(hal_lp_timer_id);
  }
}

void hal_lp_timer_irq_enable(void)
{
  // CMSIS2 timer interrupts are enabled by default when timer is started
  // This function is kept for API compatibility
  callback_enable = true;
  if (callback_pending && (hal_lp_timer_irq.callback != NULL)) {
    hal_lp_timer_irq.callback(hal_lp_timer_irq.context);
    callback_pending = false;
  }
}

void hal_lp_timer_irq_disable(void)
{
  // Stop the timer to disable interrupts
  callback_enable = false;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/**
 * @brief CMSIS2 timer callback function
 *
 * This function is called when the CMSIS2 timer expires.
 */
static void hal_lp_timer_callback(void *argument)
{
  (void)&argument; // Unused parameter

  if (callback_enable) {
    // Call the registered callback if available
    if (hal_lp_timer_irq.callback != NULL) {
      hal_lp_timer_irq.callback(hal_lp_timer_irq.context);
    }
  } else {
    callback_pending = true;
  }
}
#else // SL_CATALOG_KERNEL_PRESENT

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
sl_status_t hal_lp_timer_init(void)
{
  if (hal_lp_timer.id >= LETIMER_COUNT) {
    return SL_STATUS_INVALID_INDEX;
  }
  if (LETIMER(hal_lp_timer.id) == NULL) {
    return SL_STATUS_NOT_AVAILABLE;
  }

  // Enable the LETIMER peripheral clock
  sl_status_t status = sl_clock_manager_enable_bus_clock(hal_lp_timer.bus_clock);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Configure the LETIMER with default settings
  sl_hal_letimer_config_t config = SL_HAL_LETIMER_CONFIG_DEFAULT;

  // Set the repeat mode and enable top
  config.repeat_mode = hal_lp_timer.repeat_mode;
  config.enable_top = hal_lp_timer.enable_top;

  // Initialize the LETIMER peripheral
  sl_hal_letimer_init(LETIMER(hal_lp_timer.id), &config);

  // Enable the LETIMER peripheral
  sl_hal_letimer_enable(LETIMER(hal_lp_timer.id));

  // Enable Interrupt
  hal_lp_timer_irq_enable();

  // Get the frequency of the LETIMER peripheral
  hal_lp_timer_freq = hal_lp_timer_get_peripheral_freq();

  // Calculate the ticks per millisecond
  hal_lp_timer_ticks_per_ms = (float)hal_lp_timer_freq / 1000.0f;

  return SL_STATUS_OK;
}

void hal_lp_timer_start(const uint32_t milliseconds,
                        const hal_lp_timer_irq_t* tmr_irq)
{
  // Store the IRQ context and callback
  hal_lp_timer_irq = *tmr_irq;

  // calculate the TOP value based on the milliseconds
  uint32_t counter_value = (uint32_t)(hal_lp_timer_ticks_per_ms * (float)milliseconds);

  // Set the COMP0 value for the LETIMER
  sl_hal_letimer_set_compare(LETIMER(hal_lp_timer.id), 0, SL_LETIMER_MAX_COUNTER - counter_value);

  // Set counter as much value
  sl_hal_letimer_set_counter(LETIMER(hal_lp_timer.id), SL_LETIMER_MAX_COUNTER);

  // Start the LETIMER peripheral
  sl_hal_letimer_start(LETIMER(hal_lp_timer.id));
}

void hal_lp_timer_stop(void)
{
  // Disable the LETIMER peripheral
  sl_hal_letimer_stop(LETIMER(hal_lp_timer.id));
}

void hal_lp_timer_irq_enable(void)
{
  // Enable the COMP0 interrupt
  sl_hal_letimer_enable_interrupts(LETIMER(hal_lp_timer.id), LETIMER_IEN_COMP0);

  // Enable NVIC interrupt for LETIMER
  sl_interrupt_manager_enable_irq(hal_lp_timer.irq_num);
}

void hal_lp_timer_irq_disable(void)
{
  // Disable the COMP0 interrupt
  sl_hal_letimer_clear_interrupts(LETIMER(hal_lp_timer.id), _LETIMER_IF_MASK);
  sl_hal_letimer_disable_interrupts(LETIMER(hal_lp_timer.id), LETIMER_IEN_COMP0);

  // Disable NVIC interrupt for LETIMER
  sl_interrupt_manager_clear_irq_pending(hal_lp_timer.irq_num);
  sl_interrupt_manager_disable_irq(hal_lp_timer.irq_num);
}
uint32_t hal_lp_timer_get_peripheral_freq(void)
{
  // Get the LETIMER frequency
  sl_clock_branch_t clock_branch;

  // Get the clock branch for the LETIMER peripheral
  // This assumes LETIMER is the only low power timer used
  clock_branch = sl_device_peripheral_get_clock_branch(hal_lp_timer.peripheral);

  // Get the frequency of the clock branch
  uint32_t frequency;
  sl_clock_manager_get_clock_branch_frequency(clock_branch, &frequency);

  return frequency;
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          IRQ Handler Implementation
// -----------------------------------------------------------------------------
/**
 * @brief Common IRQ handler for the low power timer
 *
 * This function should be called from the LETIMER interrupt handler.
 */
void LETIMER_IRQ_HANDLER()
{
  // Get pending interrupts
  uint32_t flags = sl_hal_letimer_get_pending_interrupts(LETIMER(hal_lp_timer.id));

  // Check for COMP0 interrupt
  if (flags & LETIMER_IF_COMP0) {
    // Clear the interrupt flag
    if (hal_lp_timer_irq.callback != NULL) {
      hal_lp_timer_irq.callback(hal_lp_timer_irq.context);
    }
    sl_hal_letimer_clear_interrupts(LETIMER(hal_lp_timer.id), _LETIMER_IF_MASK);
  }
}
#endif // SL_CATALOG_KERNEL_PRESENT
