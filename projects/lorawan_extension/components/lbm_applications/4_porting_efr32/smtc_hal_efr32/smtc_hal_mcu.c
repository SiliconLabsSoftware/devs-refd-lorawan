/***************************************************************************//**
 * @file smtc_hal_mcu.c
 * @brief The HAL MCU Source File
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
#include "sl_component_catalog.h"
#include "smtc_hal_mcu.h"
#include "sl_sleeptimer.h"
#include "sl_power_manager.h"
#include "em_system.h"
#include "em_rmu.h"
#include "smtc_hal_gpio.h"
#include "smtc_config.h"
#include "smtc_hal_watchdog.h"
#include "smtc_hal_rng.h"
#include "smtc_hal_rtc.h"
#include "smtc_hal_lp_timer.h"
#include "smtc_hal_flash.h"
#include "smtc_hal_spi.h"
#include "smtc_hal_iadc.h"
#include "smtc_hal_crashlog.h"
#if defined (USE_FUOTA)
#include "smtc_hal_bootloader.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

#if !defined(SL_CATALOG_KERNEL_PRESENT)
/**
 * @brief Sleeptimer callback
 */
static void sleep_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);
#endif

/**
 * @brief MCU gpio initialization function
 */
static void hal_mcu_gpio_init(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

#if !defined(SL_CATALOG_KERNEL_PRESENT)
static sl_sleeptimer_timer_handle_t sleep_timer;
#endif

/**
 * @brief Flag to indicate to power manager if application can sleep
 * The application will start in EM0
 */
static bool ok_to_sleep = false;

/**
 * @brief Flag to indicate to power manager if the application should return to sleep
 * after an interrupt
 */
static sl_power_manager_on_isr_exit_t isr_ok_to_sleep = SL_POWER_MANAGER_IGNORE;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/**
 * @brief Hook for power manager
 */
bool app_is_ok_to_sleep(void)
{
  // Return false to prevent sleep mode and force EM0
  // Return true to indicate to power manager that application can sleep
  return ok_to_sleep;
}

/**
 * @brief Hook for power manager
 */
sl_power_manager_on_isr_exit_t app_sleep_on_isr_exit(void)
{
  // Flag used by power manager to determine if device can return to sleep
  // following interrupt
  return isr_ok_to_sleep;
}

void hal_mcu_critical_section_begin(uint32_t* mask)
{
  *mask = CORE_EnterCritical();
}

void hal_mcu_critical_section_end(const uint32_t* mask)
{
  CORE_ExitCritical(*mask);
}

void hal_mcu_disable_irq(void)
{
  CORE_CRITICAL_IRQ_DISABLE();
}

void hal_mcu_enable_irq(void)
{
  CORE_CRITICAL_IRQ_ENABLE();
}

void hal_mcu_init(void)
{
  uint32_t reset_cause = RMU_ResetCauseGet();
  if (reset_cause & EMU_RSTCAUSE_POR) {
    hal_crashlog_clear();
  }
  hal_mcu_gpio_init();
  hal_watchdog_init();
  hal_rtc_init();
  hal_rng_init();
  hal_lp_timer_init();
  hal_flash_init();
  hal_init_iadc0(iadcPosInputAvdd, iadcNegInputGnd);
  hal_spi_init();
#if defined (USE_FUOTA)
  hal_bootloader_get_slot_info();
#endif
}

void hal_mcu_reset(void)
{
  CORE_RESET_SYSTEM();
}
void hal_mcu_wait_ms(uint16_t milliseconds)
{
  if ( milliseconds == 0 ) {
    return;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  osDelay(milliseconds);
#else
  sl_sleeptimer_delay_millisecond(milliseconds);
#endif //SL_CATALOG_KERNEL_PRESENT
}

void hal_mcu_set_sleep_for_ms(const int32_t milliseconds)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
// Assuming no more active tasks, so just sleep by idle task
  osDelay(milliseconds);
#else
  // Set flag returned by app_is_ok_to_sleep() to indicate
  // to power manager that application can sleep
  ok_to_sleep = true;

  // Set flag returned by app_sleep_on_isr_exit() to indicate
  // to power manager that application can to sleep after
  // interrupt
  isr_ok_to_sleep = SL_POWER_MANAGER_SLEEP;

  // Sleep timer function in EM2
  sl_sleeptimer_start_timer_ms(&sleep_timer,
                               milliseconds,
                               &sleep_timer_callback,
                               (void *)NULL,
                               0,
                               SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  sl_power_manager_sleep();
#endif //SL_CATALOG_KERNEL_PRESENT
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

#if !defined(SL_CATALOG_KERNEL_PRESENT)
static void sleep_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;

  // when the timer expires, prevent return to sleep
  ok_to_sleep = false;
  isr_ok_to_sleep = SL_POWER_MANAGER_WAKEUP;
}
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
CORE_irqState_t CORE_EnterCritical(void)
{
  vPortEnterCritical();
  return 0;
}
void CORE_ExitCritical(CORE_irqState_t irqState)
{
  (void)irqState;
  vPortExitCritical();
}
#endif

static void hal_mcu_gpio_init(void)
{
  hal_gpio_irq_t busy_pin_irq = {
    .port = RADIO_BUSY_PORT,
    .pin = RADIO_BUSY_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_NONE,
    .irq_mode = SL_GPIO_INTERRUPT_NO_EDGE,
    .callback = NULL,
    .context = NULL
  };
  hal_gpio_irq_t dio_pin_irq = {
    .port = RADIO_DIOX_PORT,
    .pin = RADIO_DIOX_PIN,
    .pull_mode = RADIO_DIOX_PULL_MODE,
    .irq_mode = RADIO_DIOX_IRQ_MODE,
    .callback = NULL,
    .context = NULL
  };
  hal_gpio_init_out(RADIO_NSS_PORT, RADIO_NSS_PIN, 1);
  hal_gpio_init_in(&busy_pin_irq);
  hal_gpio_init_in(&dio_pin_irq);
  hal_gpio_init_out(RADIO_RESET_PORT, RADIO_RESET_PIN, 1);
#if LBM_ANT_SWITCH_MANUAL
  hal_gpio_irq_t rf_switch_irq = {
    .port = RADIO_ANTENNA_SWITCH_PORT,
    .pin = RADIO_ANTENNA_SWITCH_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_UP,
    .irq_mode = SL_GPIO_INTERRUPT_NO_EDGE,
    .callback = NULL,
    .context = NULL
  };
  hal_gpio_init_in(&rf_switch_irq);
#else
  hal_gpio_init_out(RADIO_ANTENNA_SWITCH_PORT, RADIO_ANTENNA_SWITCH_PIN, 1);
#endif
  hal_gpio_irq_enable();
}
