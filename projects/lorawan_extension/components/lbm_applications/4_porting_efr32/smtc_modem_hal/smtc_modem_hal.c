/***************************************************************************//**
 * @file sl_sx126x_modem_hal.c
 * @brief The SX126x Modem Hardware Abstraction Layer Source File
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
#include "smtc_modem_hal.h"
#include "smtc_hal_watchdog.h"
#include "smtc_hal_rtc.h"
#include "smtc_hal_lp_timer.h"
#include "smtc_hal_gpio.h"
#include "em_chip.h"
#include "smtc_hal_context.h"
#include "smtc_hal_rng.h"
#include "smtc_hal_iadc.h"
#include "smtc_hal_crashlog.h"
#include "smtc_hal_dbg_trace.h"
#include "smtc_config.h"

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

static hal_gpio_irq_t radio_dio_irq;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* ------------ Reset management ------------*/

void smtc_modem_hal_reset_mcu(void)
{
  // Reset the MCU
  CHIP_Reset();
}

/* ------------ Watchdog management ------------*/

void smtc_modem_hal_reload_wdog(void)
{
  // Reload the watchdog
  hal_watchdog_reload();
}

/* ------------ Time management ------------*/

uint32_t smtc_modem_hal_get_time_in_s(void)
{
  return hal_rtc_get_time_in_s();
}

uint32_t smtc_modem_hal_get_time_in_ms(void)
{
  return hal_rtc_get_time_in_ms();
}

void smtc_modem_hal_set_offset_to_test_wrapping(const uint32_t offset_to_test_wrapping)
{
  hal_rtc_set_offset_to_test_wrapping(offset_to_test_wrapping);
}

/* ------------ Timer management ------------*/

void smtc_modem_hal_start_timer(const uint32_t milliseconds,
                                void (*callback)(void* context),
                                void* context)
{
  hal_lp_timer_start(milliseconds,
                     &(hal_lp_timer_irq_t){
    .context = context,
    .callback = callback
  });
}

void smtc_modem_hal_stop_timer(void)
{
  hal_lp_timer_stop();
}

/* ------------ IRQ management ------------*/

void smtc_modem_hal_disable_modem_irq(void)
{
  hal_gpio_irq_disable();
  hal_lp_timer_irq_disable();
}

void smtc_modem_hal_enable_modem_irq(void)
{
  hal_gpio_irq_enable();
  hal_lp_timer_irq_enable();
}

/* ------------ Context saving management ------------*/

void smtc_modem_hal_context_restore(const modem_context_type_t ctx_type,
                                    uint32_t offset, uint8_t *buffer, const uint32_t size)
{
  hal_context_restore(ctx_type, offset, buffer, size);
}

void smtc_modem_hal_context_store(const modem_context_type_t ctx_type,
                                  uint32_t offset, const uint8_t *buffer, const uint32_t size)
{
  hal_context_store(ctx_type, offset, buffer, size);
}

void smtc_modem_hal_context_flash_pages_erase(
  const modem_context_type_t ctx_type, uint32_t offset, uint8_t nb_page)
{
  hal_context_flash_pages_erase(ctx_type, offset, nb_page);
}

/* ------------ Crashlog management ------------*/

void smtc_modem_hal_crashlog_store(const uint8_t *crash_string,
                                   uint8_t crash_string_length)
{
  hal_crashlog_store(crash_string, crash_string_length);
}

void smtc_modem_hal_crashlog_restore(uint8_t *crash_string,
                                     uint8_t *crash_string_length)
{
  hal_crashlog_restore(crash_string, crash_string_length);
}

void smtc_modem_hal_crashlog_set_status(bool available)
{
  hal_crashlog_set_status(available);
}

bool smtc_modem_hal_crashlog_get_status(void)
{
  return hal_crashlog_get_status();
}
/* ------------ Assert management ------------*/

void smtc_modem_hal_on_panic(uint8_t *func, uint32_t line, const char *fmt, ...)  // NOSONAR
{
  uint8_t out_buff[255] = { 0 };
  uint8_t out_len = (uint8_t)snprintf((char*) out_buff, sizeof(out_buff), "%s:%lu ",
                                      func, line);

  va_list args;
  va_start(args, fmt);
  out_len += (uint8_t)vsnprintf((char*) &out_buff[out_len], sizeof(out_buff) - out_len, fmt, args); // NOSONAR
  va_end(args);

  smtc_modem_hal_crashlog_store(out_buff, out_len);

  SMTC_HAL_TRACE_ERROR("Modem panic: %s\n", out_buff);
  smtc_modem_hal_reset_mcu();
}

/* ------------ Random management ------------*/

uint32_t smtc_modem_hal_get_random_nb_in_range(const uint32_t val_1,
                                               const uint32_t val_2)
{
  return hal_rng_get_random_in_range(val_1, val_2);
}

/* ------------ Radio environment management ------------*/

void smtc_modem_hal_irq_config_radio_irq(void (*callback)(void *context),
                                         void *context)
{
  radio_dio_irq.pin = RADIO_DIOX_PIN;
  radio_dio_irq.port = RADIO_DIOX_PORT;
  radio_dio_irq.pull_mode = RADIO_DIOX_PULL_MODE;
  radio_dio_irq.irq_mode = RADIO_DIOX_IRQ_MODE;
  radio_dio_irq.callback = callback;
  radio_dio_irq.context = context;

  hal_gpio_irq_attach(&radio_dio_irq);
}

void smtc_modem_hal_set_ant_switch(bool is_tx_on)
{
  // Control the antenna switch via GPIO. This is required for boards with an antenna switch.
  #if LBM_ANT_SWITCH_MANUAL
  hal_gpio_set_value(RADIO_ANTENNA_SWITCH_PORT, RADIO_ANTENNA_SWITCH_PIN, is_tx_on);
  #else
  (void)is_tx_on;
  #endif
}

void smtc_modem_hal_start_radio_tcxo(void)
{
  // put here the code that will start the tcxo if needed
}

void smtc_modem_hal_stop_radio_tcxo(void)
{
  // put here the code that will stop the tcxo if needed
}

uint32_t smtc_modem_hal_get_radio_tcxo_startup_delay_ms(void)
{
  // Tcxo is present on LR1110 and LR1120 evk boards
#if defined(LR11XX) || (XOSC_CFG_TCXO_ENABLE == 1)
  return 5;
#else
  return 0;
#endif
}
/* ------------ Environment management ------------*/

uint8_t smtc_modem_hal_get_battery_level(void)
{
  // Please implement according to used board
  // According to LoRaWan 1.0.4 spec:
  // 0: The end-device is connected to an external power source.
  // 1..254: Battery level, where 1 is the minimum and 254 is the maximum.
  // 255: The end-device was not able to measure the battery level

  return hal_get_battery_level();
}

int8_t smtc_modem_hal_get_board_delay_ms(void)
{
#if defined(LR1121)
  return 2;
#else
  return 1;
#endif
}
/* ------------ Trace management ------------*/

void smtc_modem_hal_print_trace(const char *fmt, ...) // NOSONAR
{
  va_list args;
  // Create a buffer to format the string
  char buffer[256];

  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args); // NOSONAR
  app_log_info("%s", buffer);
  va_end(args);
}

/* ------------ For Real Time OS compatibility  ------------*/

void smtc_modem_hal_user_lbm_irq(void)
{
  // Do nothing in case implementation is bare metal
}

/* ------------ Fuota management ------------*/
#if defined(USE_FUOTA)
uint32_t smtc_modem_hal_get_hw_version_for_fuota(void)
{
  // Example value, please fill with application value
  return 0x12345678;
}

/**
 * @brief Only use if fmp package is activated
 *
 * @return uint32_t fw version as defined in fmp Alliance package TS006-1.0.0
 */
uint32_t smtc_modem_hal_get_fw_version_for_fuota(void)
{
  // Example value, please fill with application value
  return 0x11223344;
}

/**
 * @brief Only use if fmp package is activated
 *
 * @return uint8_t fw status field as defined in fmp Alliance package TS006-1.0.0
 */
uint8_t smtc_modem_hal_get_fw_status_available_for_fuota(void)
{
  // Example value, please fill with application value
  return 3;
}

uint32_t smtc_modem_hal_get_next_fw_version_for_fuota(void)
{
  // Example value, please fill with application value
  return 0x17011973;
}
/**
 * @brief Only use if fmp package is activated
 * @param [in] fw_to_delete_version    fw_to_delete_version as described in TS006-1.0.0
 * @return uint8_t fw status field as defined in fmp Alliance package TS006-1.0.0
 */
uint8_t smtc_modem_hal_get_fw_delete_status_for_fuota(uint32_t fw_to_delete_version)
{
  if ( fw_to_delete_version != smtc_modem_hal_get_next_fw_version_for_fuota( ) ) {
    return 2;
  } else {
    return 0;
  }
}
#endif  // USE_FUOTA
