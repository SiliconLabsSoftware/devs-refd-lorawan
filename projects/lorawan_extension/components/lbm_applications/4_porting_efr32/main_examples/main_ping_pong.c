/**
 * @file main_ping_pong.c
 * @brief Main ping pong implementation file.
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*******************************************************************************
 *******************************   DEPENDENCIES   ******************************
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main_ping_pong.h"
#include "smtc_hal_gpio.h"
#include "smtc_hal_spi.h"

#include "smtc_modem_hal.h"
#include "smtc_modem_hal_dbg_trace.h"
#include "smtc_modem_api.h"

#include "smtc_modem_utilities.h"
#include "smtc_hal_mcu.h"
#include "sl_spidrv_instances.h"
#include "smtc_hal_spi.h"
#include "ralf_sx126x.h"
#include "sx126x.h"
#include "ral_defs.h"
#include "ralf.h"
#include "smtc_hal_dbg_trace.h"

#if defined (SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define PING_MSG "PING"
#define PONG_MSG "PONG"
#define PAYLOAD_LEN 8
#define PING_PONG_TIMEOUT 3000 // 3 second timeout
#define PING_PONG_PERIOD  1000 // 1 second

// Lora parameters
#define SL_LORA_SYNC_WORD 0x34 // LoRaWAN public sync word
#define SL_LORA_FREQ_HZ 868100000
#define SL_LORA_TX_POWER_DBM 14
#define SL_LORA_BANDWIDTH RAL_LORA_BW_125_KHZ
#define SL_LORA_SPREADING_FACTOR RAL_LORA_SF7
#define SL_LORA_CODING_RATE RAL_LORA_CR_4_5
#define SL_LORA_PREAMBLE_LENGTH 12
#define SL_LORA_HEADER_TYPE RAL_LORA_PKT_IMPLICIT
#define SL_LORA_CRC_ON true
#define SL_LORA_IQ_INVERTED false

// Ping device config button (BT0)
#define SL_PING_DEVICE_BTN_CONFIG_PORT gpioPortB
#define SL_PING_DEVICE_BTN_CONFIG_PIN  1
#define SL_PING_DEVICE_BTN_ID          0

// Pong device config button (BT1)
#define SL_PONG_DEVICE_BTN_CONFIG_PORT gpioPortB
#define SL_PONG_DEVICE_BTN_CONFIG_PIN  3
#define SL_PONG_DEVICE_BTN_ID          1

#define SL_BTN_DEBOUNCE_MS 50
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
const ralf_t sl_modem_rad = RALF_SX126X_INSTANTIATE(NULL);
static ralf_params_lora_t lora_params = {
  .sync_word = SL_LORA_SYNC_WORD, // LoRaWAN public sync word
  .symb_nb_timeout = 0,
  .rf_freq_in_hz = SL_LORA_FREQ_HZ,
  .output_pwr_in_dbm = SL_LORA_TX_POWER_DBM,
  .mod_params = {
    .cr = SL_LORA_CODING_RATE,
    .sf = SL_LORA_SPREADING_FACTOR,
    .bw = SL_LORA_BANDWIDTH
  },
  .pkt_params = {
    .header_type = SL_LORA_HEADER_TYPE,
    .pld_len_in_bytes = PAYLOAD_LEN,
    .crc_is_on = SL_LORA_CRC_ON,
    .invert_iq_is_on = SL_LORA_IQ_INVERTED,
    .preamble_len_in_symb = SL_LORA_PREAMBLE_LENGTH,
  }
};
static volatile ral_irq_t sli_radio_irq_flags = 0;
static volatile bool sli_is_ping_initiator = false;
static volatile bool sli_start_button_pressed = false;
static uint32_t sli_last_btn0_press_time = 0;
static uint32_t sli_last_btn1_press_time = 0;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
/**
 * @brief init_radio function
 */
static sl_status_t sli_init_radio(void);

static sl_status_t sli_verify_modem_ready(void);

static sl_status_t sli_setup_modem_for_lora_ping_pong(void);
static sl_status_t sli_lora_set_tx(void);
static sl_status_t sli_lora_set_rx(void);
static sl_status_t sli_lora_set_sleep(void);
static sl_status_t sli_get_lora_packet(void);

static void sli_button_configuration(void);
static void sli_button_callback(void* context);
static void sli_radio_irq_callback(void* context);
static void sli_ping_pong_init(void);

#ifdef SL_CATALOG_KERNEL_PRESENT
static void app_task(void *argument);
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize main Ping Pong example.
 ******************************************************************************/
void app_init(void)
{
#ifdef SL_CATALOG_KERNEL_PRESENT
  // CMSIS-RTOS v2 thread definition
  osThreadId_t app_task_id;
  (void)&app_task_id; // Unused variable
  static const osThreadAttr_t app_task_attr = {
    .name = "App Task",
    .stack_size = 2048,
    .priority = osPriorityNormal
  };

  // Initialize application task
  app_task_id = osThreadNew(app_task, NULL, &app_task_attr);
  if (app_task_id == NULL) {
    // Thread creation failed
    SMTC_HAL_TRACE_MSG("Failed to create app task\n");
    while (1) ;
  }
#endif
  sli_ping_pong_init();
}

/***************************************************************************//**
 * Main Ping Pong  process action.
 ******************************************************************************/
void app_process_action(void)
{
  smtc_modem_hal_reload_wdog();

  if (sli_radio_irq_flags != 0) {
    if (sli_radio_irq_flags & RAL_IRQ_RX_DONE) {
      sli_radio_irq_flags &= ~RAL_IRQ_RX_DONE;
      // Handle RX Done
      sli_get_lora_packet();
      if (sli_radio_irq_flags == RAL_IRQ_RX_DONE) {
        SMTC_HAL_TRACE_WARNING("Received duplicate message\n");
      }
      // Send back message
      sli_lora_set_sleep();
      hal_mcu_set_sleep_for_ms(500);
      sli_lora_set_tx();
    } else if (sli_radio_irq_flags & RAL_IRQ_TX_DONE) {
      // Handle TX Done
      SMTC_HAL_TRACE_INFO("TX Done!\n");
      sli_lora_set_sleep();
      hal_mcu_set_sleep_for_ms(500);
      sli_lora_set_rx();
      sli_radio_irq_flags &= ~RAL_IRQ_TX_DONE;
    } else if (sli_radio_irq_flags & RAL_IRQ_RX_TIMEOUT) {
      // Handle RX Timeout
      SMTC_HAL_TRACE_INFO("RX Timeout! Send msg again\n");
      sli_lora_set_sleep();
      hal_mcu_set_sleep_for_ms(500);
      sli_lora_set_tx();
      sli_radio_irq_flags &= ~RAL_IRQ_RX_TIMEOUT;
    } else if (sli_radio_irq_flags & RAL_IRQ_RX_HDR_ERROR) {
      // Handle RX Header Error
      SMTC_HAL_TRACE_ERROR("RX Header Error!\n");
      sli_radio_irq_flags &= ~RAL_IRQ_RX_HDR_ERROR;
    } else if (sli_radio_irq_flags & RAL_IRQ_RX_CRC_ERROR) {
      // Handle RX CRC Error
      SMTC_HAL_TRACE_ERROR("RX CRC Error!\n");
      sli_radio_irq_flags &= ~RAL_IRQ_RX_CRC_ERROR;
    } else if (sli_radio_irq_flags & RAL_IRQ_RX_HDR_OK) {
      // Do nothing
      sli_radio_irq_flags &= ~RAL_IRQ_RX_HDR_OK;
    } else if (sli_radio_irq_flags & RAL_IRQ_RX_PREAMBLE_DETECTED) {
      // Do nothing
      sli_radio_irq_flags &= ~RAL_IRQ_RX_PREAMBLE_DETECTED;
    } else {
      // Unhandled IRQ
      SMTC_HAL_TRACE_ERROR("Unhandled IRQ: 0x%08X\n", sli_radio_irq_flags);
    }
  }
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static sl_status_t sli_init_radio(void)
{
  ral_status_t status = RAL_STATUS_ERROR;

  // reset, init radio and put it in sleep mode
  ral_reset(&(sl_modem_rad.ral));

  status = ral_init(&(sl_modem_rad.ral));

  if (status != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ral_init() function failed\n");
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

static sl_status_t sli_verify_modem_ready(void)
{
  ral_reset(&(sl_modem_rad.ral));
  sx126x_chip_status_t chip_status;
  sx126x_status_t status;

  status = sx126x_get_status(NULL, &chip_status);

  if (status == SX126X_STATUS_OK) {
    if (chip_status.chip_mode == SX126X_CHIP_MODE_UNUSED) {
      SMTC_HAL_TRACE_ERROR("Wrong SX126X chip mode, get SX126X_CHIP_MODE_UNUSED \n");
      return SL_STATUS_FAIL;
    }
  } else {
    SMTC_HAL_TRACE_ERROR("Failed to get SX126X status\n");
  }

  return SL_STATUS_OK;
}

static sl_status_t sli_setup_modem_for_lora_ping_pong(void)
{
  // Compute LDRO
  lora_params.mod_params.ldro = ral_compute_lora_ldro(lora_params.mod_params.sf,
                                                      lora_params.mod_params.bw);

  // Setup lora params
  if (ralf_setup_lora(&sl_modem_rad, &lora_params) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ralf_setup_lora() function failed \n");
    return SL_STATUS_FAIL;
  }

  // Configure DIO IRQs
  if (ral_set_dio_irq_params(&(sl_modem_rad.ral), RAL_IRQ_ALL) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ral_set_dio_irq_params() function failed \n");
    return SL_STATUS_FAIL;
  }

  // Enable RX Boosted
  if (ral_cfg_rx_boosted(&(sl_modem_rad.ral), true) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ral_cfg_rx_boosted() function failed \n");
    return SL_STATUS_FAIL;
  }

  // Use button to start ping pong
  SMTC_HAL_TRACE_INFO("Select button 0 or 1 to start ping pong:\n");
  printf("  Button 0: Ping device\n");
  printf("  Button 1: Pong device\n");

  while (!sli_start_button_pressed) {
    // Wait for button press
    smtc_modem_hal_reload_wdog();
  }

  sli_start_button_pressed = false;
  if (sli_is_ping_initiator) {
    SMTC_HAL_TRACE_INFO("Ping device selected\n");
    // Set TX mode
    if (sli_lora_set_tx() != SL_STATUS_OK) {
      SMTC_HAL_TRACE_ERROR(" sli_lora_set_tx() function failed \n");
      return SL_STATUS_FAIL;
    }
  } else {
    SMTC_HAL_TRACE_INFO("Pong device selected\n");
    // Set RX with duty circle
    if ( sli_lora_set_rx() != SL_STATUS_OK ) {
      SMTC_HAL_TRACE_ERROR(" sli_lora_set_rx() function failed \n");
      return SL_STATUS_FAIL;
    }
  }

  return SL_STATUS_OK;
}

static sl_status_t sli_lora_set_tx(void)
{
  uint8_t tx_buf[PAYLOAD_LEN] = { 0 };
  if (sli_is_ping_initiator) {
    memcpy(tx_buf, PING_MSG, strlen(PING_MSG));
  } else {
    memcpy(tx_buf, PONG_MSG, strlen(PONG_MSG));
  }

  if (ral_set_standby(&(sl_modem_rad.ral), RAL_STANDBY_CFG_RC) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ral_set_standby() function failed \n");
    return SL_STATUS_FAIL;
  }
  if (ral_set_pkt_payload(&(sl_modem_rad.ral), tx_buf, PAYLOAD_LEN) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("ral_set_pkt_payload() function failed \n");
    return SL_STATUS_FAIL;
  }

  if (ral_set_tx(&(sl_modem_rad.ral)) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("ral_set_tx() function failed \n");
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

static sl_status_t sli_lora_set_rx(void)
{
  if (ral_set_rx_duty_cycle(&(sl_modem_rad.ral), PING_PONG_TIMEOUT, 0) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ral_set_rx_duty_cycle() function failed \n");
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

static sl_status_t sli_get_lora_packet(void)
{
  uint8_t rx_buf[PAYLOAD_LEN] = { 0 };
  uint16_t rx_len = 0;
  if (ral_get_pkt_payload(&(sl_modem_rad.ral),
                          PAYLOAD_LEN,
                          rx_buf,
                          &rx_len) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("ral_get_pkt_payload() function failed \n");
    return SL_STATUS_FAIL;
  }
  SMTC_HAL_TRACE_INFO("Received message: %.*s\n", rx_len, rx_buf);

  return SL_STATUS_OK;
}

static void sli_button_configuration(void)
{
  static uint8_t button_id[2] = { SL_PING_DEVICE_BTN_ID, SL_PONG_DEVICE_BTN_ID };
  static hal_gpio_irq_t ping_device_setup_irq = {
    .port = SL_PING_DEVICE_BTN_CONFIG_PORT,
    .pin = SL_PING_DEVICE_BTN_CONFIG_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_UP,
    .irq_mode = SL_GPIO_INTERRUPT_RISING_EDGE,
    .context = &button_id[0],
    .callback = &sli_button_callback
  };

  static hal_gpio_irq_t pong_device_setup_irq = {
    .port = SL_PONG_DEVICE_BTN_CONFIG_PORT,
    .pin = SL_PONG_DEVICE_BTN_CONFIG_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_UP,
    .irq_mode = SL_GPIO_INTERRUPT_RISING_EDGE,
    .context = &button_id[1],
    .callback = &sli_button_callback
  };

  hal_gpio_init_in(&ping_device_setup_irq);
  hal_gpio_init_in(&pong_device_setup_irq);
}

static void sli_radio_irq_callback(void* context)
{
  (void)&context;
  ral_irq_t radio_irq = 0;
  if (ral_get_irq_status(&(sl_modem_rad.ral), &radio_irq) == RAL_STATUS_OK) {
    sli_radio_irq_flags |= radio_irq;
    ral_clear_irq_status(&(sl_modem_rad.ral), RAL_IRQ_ALL);
  }
}

static void sli_button_callback(void* context)
{
  (void)&context;
  uint32_t current_time = smtc_modem_hal_get_time_in_ms();
  const uint8_t* btn_id = (const uint8_t*)context;
  sli_start_button_pressed = true;
  if (*btn_id == SL_PING_DEVICE_BTN_ID) {
    // Ping device button
    hal_gpio_clear_pending_irq(SL_PING_DEVICE_BTN_CONFIG_PIN);
    if ((current_time - sli_last_btn0_press_time) > SL_BTN_DEBOUNCE_MS) {
      sli_is_ping_initiator = true;
      sli_last_btn0_press_time = current_time;
      sli_lora_set_tx();
    }
  } else if (*btn_id == SL_PONG_DEVICE_BTN_ID) {
    // Pong device button
    hal_gpio_clear_pending_irq(SL_PONG_DEVICE_BTN_CONFIG_PIN);
    if ((current_time - sli_last_btn1_press_time) > SL_BTN_DEBOUNCE_MS) {
      sli_is_ping_initiator = false;
      sli_last_btn1_press_time = current_time;
      sli_lora_set_rx();
    }
  }
}

static void sli_ping_pong_init(void)
{
  SMTC_HAL_TRACE_INFO("LoraWAN Ping Pong without Gateway Example\n");
  hal_mcu_init();

  // Init button
  sli_button_configuration();

  // Verify modem is ready
  if (sli_verify_modem_ready() != SL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("Modem is not ready\n");
    EFM_ASSERT(NULL);
  }
  // reset and init radio
  if (sli_init_radio() != SL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("Failed to init radio\n");
    EFM_ASSERT(NULL);
  }

  // Attach IRQ callback
  smtc_modem_hal_irq_config_radio_irq(&sli_radio_irq_callback, NULL);

  if (sli_setup_modem_for_lora_ping_pong() != SL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("Failed to setup modem for LoRa ping pong\n");
    EFM_ASSERT(NULL);
  }

  sli_lora_set_sleep();
  hal_mcu_set_sleep_for_ms(PING_PONG_PERIOD);
}

static sl_status_t sli_lora_set_sleep(void)
{
  if (ral_set_standby(&(sl_modem_rad.ral), RAL_STANDBY_CFG_RC) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR(" ral_set_standby() function failed\n");
    return SL_STATUS_FAIL;
  }
  if (ral_set_sleep(&(sl_modem_rad.ral), true) != RAL_STATUS_OK) {
    SMTC_HAL_TRACE_ERROR("Failed to set radio in sleep mode\n");
    EFM_ASSERT(NULL);
    return SL_STATUS_FAIL;
  }
  // Wait to save config and switch off
  hal_mcu_wait_ms(1);

  return SL_STATUS_OK;
}

#ifdef SL_CATALOG_KERNEL_PRESENT
static void app_task(void *argument)
{
  (void)&argument;

  for (;; ) {
    app_process_action();
    osDelay(1);
  }
}
#endif
