/***************************************************************************//**
 * @file hal_rtc.c
 * @brief The LBM HAL RTC Source File
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
#include "smtc_hal_rtc.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#else // SL_CATALOG_KERNEL_PRESENT
#include "sl_hal_sysrtc.h"
#include "sl_device_peripheral.h"
#include "sl_clock_manager.h"
#include "sl_interrupt_manager.h"
#include "sl_sleeptimer_config.h"
#endif // SL_CATALOG_KERNEL_PRESENT

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#if !defined(SL_CATALOG_KERNEL_PRESENT)
#if SL_SLEEPTIMER_PERIPHERAL == SL_SLEEPTIMER_PERIPHERAL_SYSRTC
#error "SYSRTC is occupied by LBM HAL, please use another peripheral for sleep timer."
#endif

// Minimum difference between current count value and what the comparator of the timer can be set to.
// 1 tick is add to the minimum diff for the algorithm of compensation for the IRQ handler that
// triggers when CNT == compare value + 1.
#define SMTC_HAL_RTC_COMPARE_MIN_DIFF (2 + 1)
#define SMTC_HAL_RTC_TMR_WIDTH (_SYSRTC_CNT_MASK)
#endif // SL_CATALOG_KERNEL_PRESENT

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**
 * @brief Get the current RTC counter value.
 * @return The current RTC counter value.
 */
static uint32_t hal_rtc_get_counter(void);

/**
 * @brief Get the current RTC timer frequency.
 * @return The current RTC timer frequency in Hz.
 */
static uint32_t hal_get_timer_frequency(void);

#if !defined(SL_CATALOG_KERNEL_PRESENT)
/**
 * @brief Convert milliseconds to RTC ticks.
 * @param[in] time_ms The time in milliseconds to convert.
 * @return The equivalent time in RTC ticks.
 */
static uint32_t hal_rtc_ms_to_ticks(uint16_t time_ms);
#endif

__STATIC_INLINE bool is_power_of_2(uint32_t nbr)
{
  if ((nbr != 0u) && ((nbr & (nbr - 1u)) == 0u)) {
    return true;
  } else {
    return false;
  }
}
__STATIC_INLINE uint32_t div_to_log2(uint32_t div)
{
  return 31UL - __CLZ(div);  // Count leading zeroes and "reverse" result.
}

// __STATIC_INLINE uint32_t get_time_diff(uint32_t a, uint32_t b)
// {
//   return a - b;
// }
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void hal_rtc_init(void)
{
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  // Initialize SYSRTC configuration with default values
  sl_hal_sysrtc_config_t sysrtc_config = SYSRTC_CONFIG_DEFAULT;
  sl_hal_sysrtc_group_config_t group_config = SYSRTC_GROUP_CONFIG_DEFAULT;

  // Enable SYSRTC bus clock
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_SYSRTC0);

  // Make sure the bus clock enabling is done
  __DSB();

  // Initialize SYSRTC with default configuration
  sl_hal_sysrtc_init(&sysrtc_config);

  // Disable compare channel 0 for 1ms timing
  group_config.compare_channel0_enable = false;

  sl_hal_sysrtc_init_group(0u, &group_config);

  // Clear and disable all group interrupts (polling-based approach)
  sl_hal_sysrtc_disable_group_interrupts(0u, _SYSRTC_GRP0_IEN_MASK);
  sl_hal_sysrtc_clear_group_interrupts(0u, _SYSRTC_GRP0_IF_MASK);

  // Enable SYSRTC and start counter from 0
  sl_hal_sysrtc_enable();
  sl_hal_sysrtc_set_counter(0u);

  // Disable interrupts of the RTC group
  sl_interrupt_manager_clear_irq_pending(SYSRTC_APP_IRQn);
  sl_interrupt_manager_disable_irq(SYSRTC_APP_IRQn);

  sl_hal_sysrtc_start();
#endif // SL_CATALOG_KERNEL_PRESENT
}

uint32_t hal_rtc_get_time_in_s(void)
{
  return hal_rtc_get_time_in_ms() / 1000;
}

uint32_t hal_rtc_get_time_in_ms(void)
{
  uint32_t time_ms = 0;
  uint32_t frequency = hal_get_timer_frequency();

  if (frequency != 0u) {
    uint32_t current_ticks = hal_rtc_get_counter();
    if (is_power_of_2(frequency)) {
      // If frequency is a power of 2, we can use bitwise operations
      time_ms = (uint32_t)(((uint64_t)current_ticks * (uint64_t)1000u) >> div_to_log2(frequency));
    } else {
      // Otherwise, we use the division method
      time_ms = (uint32_t)(((uint64_t)current_ticks * (uint64_t)1000u) / frequency);
    }
  }
  return time_ms;
}

void hal_rtc_set_offset_to_test_wrapping(uint32_t offset_to_test_wrapping)
{
  (void)offset_to_test_wrapping;
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  uint32_t tick_to_wrap = hal_rtc_ms_to_ticks((uint16_t)offset_to_test_wrapping);
  sl_hal_sysrtc_set_counter(UINT32_MAX - tick_to_wrap);
#endif // SL_CATALOG_KERNEL_PRESENT
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static uint32_t hal_rtc_get_counter(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  return osKernelGetTickCount();
#else // SL_CATALOG_KERNEL_PRESENT
  return sl_hal_sysrtc_get_counter();
#endif  // SL_CATALOG_KERNEL_PRESENT
}

static uint32_t hal_get_timer_frequency(void)
{
  uint32_t frequency;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  frequency = osKernelGetTickFreq();
#else // SL_CATALOG_KERNEL_PRESENT

  sl_clock_branch_t clock_branch;

  clock_branch = sl_device_peripheral_get_clock_branch(SL_PERIPHERAL_SYSRTC0);
  sl_clock_manager_get_clock_branch_frequency(clock_branch, &frequency);
#endif  // SL_CATALOG_KERNEL_PRESENT
  return frequency;
}
#if !defined(SL_CATALOG_KERNEL_PRESENT)
static uint32_t hal_rtc_ms_to_ticks(uint16_t time_ms)
{
  uint32_t frequency = hal_get_timer_frequency();
  return (uint32_t)((((uint64_t)time_ms * frequency) + 999) / 1000);
}
#endif // SL_CATALOG_KERNEL_PRESENT
