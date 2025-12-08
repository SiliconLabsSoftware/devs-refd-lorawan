/**
 * @file main_lctt_certif.c
 * @brief Main LCTT certification file.
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
#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type
#include <string.h>

#include "main_lctt_certif.h"
#include "smtc_hal_gpio.h"

#include "smtc_modem_test_api.h"
#include "smtc_modem_api.h"
#include "smtc_modem_utilities.h"
#include "smtc_modem_hal.h"

#include "smtc_hal_dbg_trace.h"
#include "smtc_hal_mcu.h"
#include "smtc_modem_relay_api.h"

#include "example_options.h"
#if defined (SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
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

/**
 * @brief Helper macro that returned a human-friendly message if a command does not return SMTC_MODEM_RC_OK
 *
 * @remark The macro is implemented to be used with functions returning a @ref smtc_modem_return_code_t
 *
 * @param[in] rc  Return code
 */

/**
 * @brief LCTT certification stack ID
 */
#define STACK_ID 0

/**
 * @brief Watchdog counter reload value during sleep (The period must be lower than MCU watchdog period (here 32s))
 */
#define WATCHDOG_RELOAD_PERIOD_MS 20000

/**
 * @brief Periodical uplink alarm delay in seconds
 */
#define PERIODICAL_UPLINK_DELAY_S 10

/**
 * @brief User button context
 */
#define USER_BUTTON_PORT gpioPortB
#define USER_BUTTON_PIN  1  // BTN0
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
/**
 * @brief Stack credentials
 */
#if !defined(USE_LR11XX_CREDENTIALS)
static const uint8_t user_dev_eui[8]      = USER_LORAWAN_DEVICE_EUI;
static const uint8_t user_join_eui[8]     = USER_LORAWAN_JOIN_EUI;
static const uint8_t user_gen_app_key[16] = USER_LORAWAN_GEN_APP_KEY;
static const uint8_t user_app_key[16]     = USER_LORAWAN_APP_KEY;
#endif

static uint8_t                  rx_payload[SMTC_MODEM_MAX_LORAWAN_PAYLOAD_LENGTH] = { 0 };  // Buffer for rx payload
static uint8_t                  rx_payload_size = 0;      // Size of the payload in the rx_payload buffer
static smtc_modem_dl_metadata_t rx_metadata     = { 0 };  // Metadata of downlink
static uint8_t                  rx_remaining    = 0;      // Remaining downlink payload in modem

static volatile bool user_button_is_press = false;  // Flag for button status
static uint32_t      uplink_counter       = 0;      // uplink raising counter

static bool certif_running = false;

static volatile uint32_t sleep_time_ms = 0;
/**
 * @brief Internal credentials
 */
#if defined(USE_LR11XX_CREDENTIALS)
static uint8_t chip_eui[SMTC_MODEM_EUI_LENGTH] = { 0 };
static uint8_t chip_pin[SMTC_MODEM_PIN_LENGTH] = { 0 };
#endif
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
/**
 * @brief User callback for modem event
 *
 *  This callback is called every time an event ( see smtc_modem_event_t ) appears in the modem.
 *  Several events may have to be read from the modem when this callback is called.
 */
static void modem_event_callback(void);

/**
 * @brief User callback for button EXTI
 *
 * @param context Define by the user at the init
 */
static void user_button_callback(const void* context);

/**
 * @brief Handle action taken if button is pushed
 *
 */
static void main_handle_push_button(void);

/**
 * @brief Send the 32bits uplink counter on chosen port
 */
static void send_uplink_counter_on_port(uint8_t port);

/**
 * @brief Initialize LCTT certification application
 */

static void lctt_certif_init(void);

#ifdef SL_CATALOG_KERNEL_PRESENT
static void app_task(void *argument);
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize main LCTT certification tests.
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
  lctt_certif_init();
}

/***************************************************************************//**
 * Main LCTT certification process action.
 ******************************************************************************/
void app_process_action(void)
{
  // Check button
  if (user_button_is_press == true) {
    SMTC_HAL_TRACE_INFO("Button pushed\n");
    user_button_is_press = false;
    main_handle_push_button();
  }

  // Modem process launch
  sleep_time_ms = smtc_modem_run_engine();

  // Atomically check sleep conditions (button was not pressed)
  if ((user_button_is_press == false) && (smtc_modem_is_irq_flag_pending() == false)) {
    smtc_modem_hal_reload_wdog();
  }
  smtc_modem_hal_reload_wdog();
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/
static void modem_event_callback(void)
{
  SMTC_HAL_TRACE_MSG_COLOR("Modem event callback\n", HAL_DBG_TRACE_COLOR_BLUE);

  smtc_modem_event_t current_event;
  uint8_t            event_pending_count;
  uint8_t            stack_id = STACK_ID;

  // Continue to read modem event until all event has been processed
  do {
    // Read modem event
    ASSERT_SMTC_MODEM_RC(smtc_modem_get_event(&current_event, &event_pending_count));

    switch ( current_event.event_type ) {
      case SMTC_MODEM_EVENT_RESET:
      {
        SMTC_HAL_TRACE_INFO("Event received: RESET\n");

#if !defined(USE_LR11XX_CREDENTIALS)
        // Set user credentials
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_deveui(stack_id, user_dev_eui));
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_joineui(stack_id, user_join_eui));
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_appkey(stack_id, user_gen_app_key));
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_nwkkey(stack_id, user_app_key));
#else
        // Get internal credentials
        ASSERT_SMTC_MODEM_RC(smtc_modem_get_chip_eui(stack_id, chip_eui));
        SMTC_HAL_TRACE_ARRAY("CHIP_EUI", chip_eui, SMTC_MODEM_EUI_LENGTH);
        ASSERT_SMTC_MODEM_RC(smtc_modem_get_pin(stack_id, chip_pin));
        SMTC_HAL_TRACE_ARRAY("CHIP_PIN", chip_pin, SMTC_MODEM_PIN_LENGTH);
#endif
        // Set user region
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_region(stack_id, MODEM_EXAMPLE_REGION));
        ASSERT_SMTC_MODEM_RC(smtc_modem_get_certification_mode(stack_id, &certif_running));
        if (certif_running == false) {
          ASSERT_SMTC_MODEM_RC(smtc_modem_join_network(stack_id));
        }
        break;
      }

      case SMTC_MODEM_EVENT_ALARM:
        SMTC_HAL_TRACE_INFO("Event received: ALARM\n");
        if (certif_running == true) {
          ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_clear_timer());
        } else {
          // Send periodical uplink on port 101
          send_uplink_counter_on_port(101);
          ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_start_timer(PERIODICAL_UPLINK_DELAY_S));
        }
        break;

      case SMTC_MODEM_EVENT_JOINED:
        SMTC_HAL_TRACE_INFO("Event received: JOINED\n");
        SMTC_HAL_TRACE_INFO("Modem is now joined \n");
        if (certif_running == false) {
          // start periodical uplink alarm
          ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_start_timer(PERIODICAL_UPLINK_DELAY_S));
        }
        break;

      case SMTC_MODEM_EVENT_TXDONE:
        SMTC_HAL_TRACE_INFO("Event received: TXDONE\n");
        SMTC_HAL_TRACE_INFO("Transmission done \n");
        break;

      case SMTC_MODEM_EVENT_DOWNDATA:
        SMTC_HAL_TRACE_INFO("Event received: DOWNDATA\n");
        // Get downlink data
        ASSERT_SMTC_MODEM_RC(
          smtc_modem_get_downlink_data(rx_payload, &rx_payload_size, &rx_metadata, &rx_remaining));
        SMTC_HAL_TRACE_PRINTF("Data received on port %u\n", rx_metadata.fport);
        SMTC_HAL_TRACE_ARRAY("Received payload", rx_payload, rx_payload_size);
        break;

      case SMTC_MODEM_EVENT_JOINFAIL:
        SMTC_HAL_TRACE_INFO("Event received: JOINFAIL\n");
        break;

      case SMTC_MODEM_EVENT_ALCSYNC_TIME:
        SMTC_HAL_TRACE_INFO("Event received: ALCSync service TIME\n");
        break;

      case SMTC_MODEM_EVENT_LINK_CHECK:
        SMTC_HAL_TRACE_INFO("Event received: LINK_CHECK\n");
        break;

      case SMTC_MODEM_EVENT_CLASS_B_PING_SLOT_INFO:
        SMTC_HAL_TRACE_INFO("Event received: CLASS_B_PING_SLOT_INFO\n");
        break;

      case SMTC_MODEM_EVENT_CLASS_B_STATUS:
        SMTC_HAL_TRACE_INFO("Event received: CLASS_B_STATUS\n");
        break;

      case SMTC_MODEM_EVENT_LORAWAN_MAC_TIME:
        SMTC_HAL_TRACE_WARNING("Event received: LORAWAN MAC TIME\n");
        break;

      case SMTC_MODEM_EVENT_LORAWAN_FUOTA_DONE:
      {
        bool status = current_event.event_data.fuota_status.successful;
        if (status == true) {
          SMTC_HAL_TRACE_INFO("Event received: FUOTA SUCCESSFUL\n");
        } else {
          SMTC_HAL_TRACE_WARNING("Event received: FUOTA FAIL\n");
        }
        break;
      }

      case SMTC_MODEM_EVENT_NO_MORE_MULTICAST_SESSION_CLASS_C:
        SMTC_HAL_TRACE_INFO("Event received: MULTICAST CLASS_C STOP\n");
        break;

      case SMTC_MODEM_EVENT_NO_MORE_MULTICAST_SESSION_CLASS_B:
        SMTC_HAL_TRACE_INFO("Event received: MULTICAST CLASS_B STOP\n");
        break;

      case SMTC_MODEM_EVENT_NEW_MULTICAST_SESSION_CLASS_C:
        SMTC_HAL_TRACE_INFO("Event received: New MULTICAST CLASS_C\n");
        break;

      case SMTC_MODEM_EVENT_NEW_MULTICAST_SESSION_CLASS_B:
        SMTC_HAL_TRACE_INFO("Event received: New MULTICAST CLASS_B\n");
        break;

      case SMTC_MODEM_EVENT_FIRMWARE_MANAGEMENT:
        SMTC_HAL_TRACE_INFO("Event received: FIRMWARE_MANAGEMENT\n");
        if (current_event.event_data.fmp.status == SMTC_MODEM_EVENT_FMP_REBOOT_IMMEDIATELY) {
          smtc_modem_hal_reset_mcu( );
        }
        break;

      case SMTC_MODEM_EVENT_STREAM_DONE:
        SMTC_HAL_TRACE_INFO("Event received: STREAM_DONE\n");
        break;

      case SMTC_MODEM_EVENT_UPLOAD_DONE:
        SMTC_HAL_TRACE_INFO("Event received: UPLOAD_DONE\n");
        break;

      case SMTC_MODEM_EVENT_DM_SET_CONF:
        SMTC_HAL_TRACE_INFO("Event received: DM_SET_CONF\n");
        break;

      case SMTC_MODEM_EVENT_MUTE:
        SMTC_HAL_TRACE_INFO("Event received: MUTE\n");
        break;

      case SMTC_MODEM_EVENT_RELAY_TX_DYNAMIC: //!< Relay TX dynamic mode has enable or disable the WOR protocol
        SMTC_HAL_TRACE_INFO("Event received: RELAY_TX_DYNAMIC\n");
        break;

      case SMTC_MODEM_EVENT_RELAY_TX_MODE: //!< Relay TX activation has been updated
        SMTC_HAL_TRACE_INFO("Event received: RELAY_TX_MODE\n");
        break;

      case SMTC_MODEM_EVENT_RELAY_TX_SYNC: //!< Relay TX synchronisation has changed
        SMTC_HAL_TRACE_INFO("Event received: RELAY_TX_SYNC\n");
        break;
      case SMTC_MODEM_EVENT_RELAY_RX_RUNNING:
        SMTC_HAL_TRACE_INFO("Event received: RELAY_RX_RUNNING\n");
#if defined(ADD_CSMA)
        bool csma_state = false;
        ASSERT_SMTC_MODEM_RC(smtc_modem_csma_get_state(STACK_ID, &csma_state) );
        if ( (current_event.event_data.relay_rx.status == true) && (csma_state == true) ) {
          // Disable CSMA when Relay Rx Is enabled by network
          ASSERT_SMTC_MODEM_RC(smtc_modem_csma_set_state(STACK_ID, false) );
        }
#if defined(ENABLE_CSMA_BY_DEFAULT)
        if ( current_event.event_data.relay_rx.status == false ) {
          ASSERT_SMTC_MODEM_RC(smtc_modem_csma_set_state(STACK_ID, true) );
        }
#endif  // ENABLE_CSMA_BY_DEFAULT
#endif  // ADD_CSMA

        break;
      case SMTC_MODEM_EVENT_REGIONAL_DUTY_CYCLE:
        SMTC_HAL_TRACE_INFO("Event received: DUTY_CYCLE\n");
        break;

      default:
        SMTC_HAL_TRACE_ERROR("Unknown event %u\n", current_event.event_type);
        break;
    }
  } while (event_pending_count > 0);
}

static void user_button_callback(const void* context)
{
  (void) context;  // Not used in the example - avoid warning

  static uint32_t last_press_timestamp_ms = 0;

  // Debounce the button press, avoid multiple triggers
  if ((int32_t)(smtc_modem_hal_get_time_in_ms() - last_press_timestamp_ms) > 500 ) {
    last_press_timestamp_ms = smtc_modem_hal_get_time_in_ms();
    user_button_is_press    = true;
  }
}
static void main_handle_push_button(void)
{
  if (certif_running == true) {
    smtc_modem_set_certification_mode(STACK_ID, false);
    ASSERT_SMTC_MODEM_RC(smtc_modem_leave_network(STACK_ID) );
    ASSERT_SMTC_MODEM_RC(smtc_modem_join_network(STACK_ID) );
    certif_running = false;
  } else {
    smtc_modem_set_certification_mode(STACK_ID, true);
    certif_running = true;
  }
}

static void send_uplink_counter_on_port(uint8_t port)
{
  // Send uplink counter on port 102
  uint8_t buff[4] = { 0 };
  buff[0]         = (uplink_counter >> 24) & 0xFF;
  buff[1]         = (uplink_counter >> 16) & 0xFF;
  buff[2]         = (uplink_counter >> 8) & 0xFF;
  buff[3]         = (uplink_counter & 0xFF);
  ASSERT_SMTC_MODEM_RC(smtc_modem_request_uplink(STACK_ID, port, false, buff, 4));
  uplink_counter++;
}

static void lctt_certif_init(void)
{
  // Initialize peripherals
  hal_mcu_init();

  // Configure user button
  static hal_gpio_irq_t user_button_config = {
    .port = USER_BUTTON_PORT,
    .pin = USER_BUTTON_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_UP,
    .irq_mode = SL_GPIO_INTERRUPT_RISING_EDGE,
    .context = NULL,
    .callback = &user_button_callback
  };
  hal_gpio_init_in(&user_button_config);

  // Init the modem and use modem_event_callback as event callback, please note that the callback will be
  // called immediately after the first call to smtc_modem_run_engine because of the reset detection
  smtc_modem_init(&modem_event_callback);

  SMTC_HAL_TRACE_INFO("Certification example is starting\n");
  SMTC_HAL_TRACE_INFO("Push button to enable/disable certification\n");
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
