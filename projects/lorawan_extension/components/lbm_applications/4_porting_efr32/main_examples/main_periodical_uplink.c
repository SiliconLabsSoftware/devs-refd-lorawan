/*!
 * \file      main_periodical_uplink.c
 *
 * \brief     main program for periodical uplink tests example
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

#include "main_periodical_uplink.h"
#include "smtc_modem_api.h"
#include "smtc_hal_gpio.h"
#include "smtc_modem_hal.h"
#include "smtc_hal_spi.h"
#include "smtc_modem_utilities.h"
#include "smtc_modem_test_api.h"
#include "smtc_modem_relay_api.h"
#include "example_options.h"
#include "smtc_hal_watchdog.h"
#include "smtc_config.h"
#include "smtc_hal_mcu.h"
#include "smtc_hal_dbg_trace.h"
#if defined (USE_FUOTA)
#include "smtc_hal_bootloader.h"
#endif
#if defined (SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/**
 * Stack id value (multistacks modem is not yet available)
 */
#define STACK_ID 0

#define EXTI_BUTTON_PORT gpioPortB
#define EXTI_BUTTON_PIN  1
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
 * @brief Watchdog counter reload value during sleep (The period must be lower than MCU watchdog period)
 */
#define WATCHDOG_RELOAD_PERIOD_MS 20000

/**
 * @brief Periodical uplink alarm delay in seconds
 */
#ifndef PERIODICAL_UPLINK_DELAY_S
#define PERIODICAL_UPLINK_DELAY_S 10
#endif

#ifndef DELAY_FIRST_MSG_AFTER_JOIN
#define DELAY_FIRST_MSG_AFTER_JOIN 10
#endif
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static uint8_t                  rx_payload[SMTC_MODEM_MAX_LORAWAN_PAYLOAD_LENGTH] = { 0 };  // Buffer for rx payload
static uint8_t                  rx_payload_size = 0;      // Size of the payload in the rx_payload buffer
static smtc_modem_dl_metadata_t rx_metadata     = { 0 };  // Metadata of downlink
static uint8_t                  rx_remaining    = 0;      // Remaining downlink payload in modem

static uint32_t                 uplink_counter  = 0;      // uplink raising counter
/**
 * @brief Stack credentials
 */
#if !defined(USE_LR11XX_CREDENTIALS)
static const uint8_t user_dev_eui[8]      = USER_LORAWAN_DEVICE_EUI;
static const uint8_t user_join_eui[8]     = USER_LORAWAN_JOIN_EUI;
static const uint8_t user_gen_app_key[16] = USER_LORAWAN_GEN_APP_KEY;
static const uint8_t user_app_key[16]     = USER_LORAWAN_APP_KEY;
#endif

static volatile bool user_button_is_press = false;  // Flag for button status

#if defined(USE_RELAY_TX)
static smtc_modem_relay_tx_config_t relay_config = { 0 };
#endif

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

/*******************************************************************************
 **********************   PRIVATE FUNCTION DEFINES   ***************************
 ******************************************************************************/

/**
 * @brief User callback for modem event
 *
 *  This callback is called every time an event ( see smtc_modem_event_t ) appears in the modem.
 *  Several events may have to be read from the modem when this callback is called.
 */
static void modem_event_callback(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**
 * @brief User callback for button EXTI
 *
 * @param context Define by the user at the init
 */
static void user_button_callback(void* context);

/**
 * @brief Send the 32bits uplink counter on chosen port
 */
static void send_uplink_counter_on_port(uint8_t port);

/**
 * @brief Converts GPS time (seconds since GPS epoch) to a human-readable date string.
 *
 * This function converts a GPS time value (seconds since January 6, 1980, 00:00:00 UTC)
 * to a Unix timestamp and then extracts the corresponding date and time components.
 * Note: This conversion does not account for leap seconds. The result is in UTC.
 *
 * @param gps_time_s GPS time in seconds since GPS epoch (Jan 6, 1980, 00:00:00 UTC)
 */
static void gps_time_to_date_string(uint32_t gps_time_s);
/**
 * @brief Initialize main periodical uplink
 */
static void periodical_uplink_init(void);

#ifdef SL_CATALOG_KERNEL_PRESENT
static void app_task(void *argument);
#endif

/***************************************************************************//**
 * Initialize main periodical uplink.
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
  periodical_uplink_init();
}

/***************************************************************************//**
 * Main periodical uplink process action.
 ******************************************************************************/
void app_process_action(void)
{
  // Check button
  if ( user_button_is_press == true ) {
    user_button_is_press = false;

    smtc_modem_status_mask_t status_mask = 0;
    smtc_modem_get_status(STACK_ID, &status_mask);
    // Check if the device has already joined a network
    if ( (status_mask & SMTC_MODEM_STATUS_JOINED) == SMTC_MODEM_STATUS_JOINED ) {
      // Send the uplink counter on port 102
      send_uplink_counter_on_port(102);
    }
  }

  // Modem process launch
  smtc_modem_run_engine( );

  if ( (user_button_is_press == false) && (smtc_modem_is_irq_flag_pending( ) == false) ) {
    smtc_modem_hal_reload_wdog();
  }
  smtc_modem_hal_reload_wdog();
}

static void modem_event_callback(void)
{
  SMTC_HAL_TRACE_MSG_COLOR("Modem event callback\n", HAL_DBG_TRACE_COLOR_BLUE);

  smtc_modem_event_t current_event;
  uint8_t            event_pending_count;
  uint8_t            stack_id = STACK_ID;

  // Continue to read modem event until all event has been processed
  do {
    // Read modem event
    ASSERT_SMTC_MODEM_RC(smtc_modem_get_event(&current_event, &event_pending_count) );

    switch ( current_event.event_type ) {
      case SMTC_MODEM_EVENT_RESET:
        SMTC_HAL_TRACE_INFO("Event received: RESET\n");
#if !defined(USE_LR11XX_CREDENTIALS)
        // Set user credentials
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_deveui(stack_id, user_dev_eui));
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_joineui(stack_id, user_join_eui));
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_appkey(stack_id, user_gen_app_key));
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_nwkkey(stack_id, user_app_key));
#else
        // Get internal credentials
        ASSERT_SMTC_MODEM_RC(smtc_modem_get_chip_eui(stack_id, chip_eui) );
        SMTC_HAL_TRACE_ARRAY("CHIP_EUI", chip_eui, SMTC_MODEM_EUI_LENGTH);
        ASSERT_SMTC_MODEM_RC(smtc_modem_get_pin(stack_id, chip_pin) );
        SMTC_HAL_TRACE_ARRAY("CHIP_PIN", chip_pin, SMTC_MODEM_PIN_LENGTH);
#endif
        // Set user region
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_region(stack_id, MODEM_EXAMPLE_REGION));
        // Schedule a Join LoRaWAN network
        #if defined(USE_RELAY_TX)
        // by default when relay mode is activated , CSMA is also activated by default to at least protect the WOR
        // transmission
        // if you want to disable the csma please uncomment the next line
        // ASSERT_SMTC_MODEM_RC(smtc_modem_csma_set_state (stack_id,false));

        relay_config.second_ch_enable = false;

        // The RelayModeActivation field indicates how the end-device SHOULD manage the relay mode.
        relay_config.activation =
          SMTC_MODEM_RELAY_TX_ACTIVATION_MODE_ENABLE;        // SMTC_MODEM_RELAY_TX_ACTIVATION_MODE_DYNAMIC;

        // number_of_miss_wor_ack_to_switch_in_nosync_mode  field indicates that the
        // relay mode SHALL be restart in no sync mode when it does not receive a WOR ACK frame after
        // number_of_miss_wor_ack_to_switch_in_nosync_mode consecutive uplinks.
        relay_config.number_of_miss_wor_ack_to_switch_in_nosync_mode = 3;

        // smart_level field indicates that the
        // relay mode SHALL be enabled if the end-device does not receive a valid downlink after smart_level
        // consecutive uplinks.
        relay_config.smart_level = 8;

        // The BackOff field indicates how the end-device SHALL behave when it does not receive
        // a WOR ACK frame.
        // BackOff Description
        // 0 Always send a LoRaWAN uplink
        // 1..63 Send a LoRaWAN uplink after X WOR frames without a WOR ACK
        relay_config.backoff = 0;      // 4;
        ASSERT_SMTC_MODEM_RC(smtc_modem_relay_tx_enable(stack_id, &relay_config) );
#endif
        ASSERT_SMTC_MODEM_RC(smtc_modem_join_network(stack_id));

        break;

      case SMTC_MODEM_EVENT_ALARM:
        SMTC_HAL_TRACE_INFO("Event received: ALARM\n");
        // Send periodical uplink on port 101
        send_uplink_counter_on_port(101);
        // Restart periodical uplink alarm
        ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_start_timer(PERIODICAL_UPLINK_DELAY_S));

        break;

      case SMTC_MODEM_EVENT_JOINED:
        SMTC_HAL_TRACE_INFO("Event received: JOINED\n");
        SMTC_HAL_TRACE_INFO("Modem is now joined \n");
        ASSERT_SMTC_MODEM_RC(smtc_modem_trig_lorawan_mac_request(stack_id, SMTC_MODEM_LORAWAN_MAC_REQ_DEVICE_TIME));
        // Send first periodical uplink on port 101
        send_uplink_counter_on_port(101);
        // start periodical uplink alarm
        ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_start_timer(DELAY_FIRST_MSG_AFTER_JOIN));
        break;

      case SMTC_MODEM_EVENT_TXDONE:
        SMTC_HAL_TRACE_INFO("Event received: TXDONE\n");
        SMTC_HAL_TRACE_INFO("Transmission done \n");
        break;

      case SMTC_MODEM_EVENT_DOWNDATA:
        SMTC_HAL_TRACE_INFO("Event received: DOWNDATA\n");
        // Get downlink data
        ASSERT_SMTC_MODEM_RC(
          smtc_modem_get_downlink_data(rx_payload, &rx_payload_size, &rx_metadata, &rx_remaining) );
        SMTC_HAL_TRACE_PRINTF("Data received on port %u\n", rx_metadata.fport);
        SMTC_HAL_TRACE_ARRAY("Received payload", rx_payload, rx_payload_size);
        break;

      case SMTC_MODEM_EVENT_ALCSYNC_TIME:
        SMTC_HAL_TRACE_INFO("Event received: ALCSync service TIME\n");
#if defined(ADD_SMTC_ALC_SYNC)
        uint32_t gps_time_s;
        ASSERT_SMTC_MODEM_RC(smtc_modem_get_alcsync_time(stack_id, &gps_time_s));
        gps_time_to_date_string(gps_time_s);
#endif
        break;

      case SMTC_MODEM_EVENT_JOINFAIL:
        SMTC_HAL_TRACE_INFO("Event received: JOINFAIL\n");
        break;

      case SMTC_MODEM_EVENT_CLASS_B_PING_SLOT_INFO:
        SMTC_HAL_TRACE_INFO("Event received: CLASS_B_PING_SLOT_INFO\n");
        break;

      case SMTC_MODEM_EVENT_LINK_CHECK:
        SMTC_HAL_TRACE_INFO("Event received: LINK_CHECK\n");
        break;

      case SMTC_MODEM_EVENT_LORAWAN_MAC_TIME:
        SMTC_HAL_TRACE_WARNING("Event received: LORAWAN MAC TIME\n");
        if (current_event.event_data.lorawan_mac_time.status == SMTC_MODEM_EVENT_MAC_REQUEST_ANSWERED) {
          // Handle MAC request answered
          uint32_t gps_time_s;
          uint32_t gps_fractional_s;
          smtc_modem_get_lorawan_mac_time(stack_id, &gps_time_s, &gps_fractional_s);
          gps_time_to_date_string(gps_time_s);
        }
        break;

      case SMTC_MODEM_EVENT_CLASS_B_STATUS:
        SMTC_HAL_TRACE_INFO("Event received: CLASS_B_STATUS\n");
        break;

      case SMTC_MODEM_EVENT_LORAWAN_FUOTA_DONE:
      {
        bool status = current_event.event_data.fuota_status.successful;
        if ( status == true ) {
          SMTC_HAL_TRACE_INFO("Event received: FUOTA SUCCESSFUL\n");
#if defined (USE_FUOTA)
          if (hal_bootloader_verify_application() == true) {
            hal_bootloader_setImage_and_reboot();
          } else {
            SMTC_HAL_TRACE_WARNING("New application not verified, aborting reboot\n");
          }
#endif
        } else {
          SMTC_HAL_TRACE_WARNING("Event received: FUOTA FAIL\n");
        }
        break;
      }

      case SMTC_MODEM_EVENT_NO_MORE_MULTICAST_SESSION_CLASS_B:
        SMTC_HAL_TRACE_INFO("Event received: MULTICAST CLASS_B STOP\n");
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_class(stack_id, SMTC_MODEM_CLASS_A));
        ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_start_timer(PERIODICAL_UPLINK_DELAY_S));
        break;

      case SMTC_MODEM_EVENT_NO_MORE_MULTICAST_SESSION_CLASS_C:
        SMTC_HAL_TRACE_INFO("Event received: MULTICAST CLASS_C STOP\n");
        ASSERT_SMTC_MODEM_RC(smtc_modem_set_class(stack_id, SMTC_MODEM_CLASS_A));
        ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_start_timer(PERIODICAL_UPLINK_DELAY_S));
        break;

      case SMTC_MODEM_EVENT_NEW_MULTICAST_SESSION_CLASS_B:
        SMTC_HAL_TRACE_INFO("Event received: New MULTICAST CLASS_B\n");
#if defined (USE_FUOTA)
        ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_clear_timer());
        hal_bootloader_erase_slot_if_needed();
#endif
        break;

      case SMTC_MODEM_EVENT_NEW_MULTICAST_SESSION_CLASS_C:
        SMTC_HAL_TRACE_INFO("Event received: New MULTICAST CLASS_C \n");
#if defined (USE_FUOTA)
        ASSERT_SMTC_MODEM_RC(smtc_modem_alarm_clear_timer());
        hal_bootloader_erase_slot_if_needed();
#endif
        break;

      case SMTC_MODEM_EVENT_FIRMWARE_MANAGEMENT:
        SMTC_HAL_TRACE_INFO("Event received: FIRMWARE_MANAGEMENT\n");
        if ( current_event.event_data.fmp.status == SMTC_MODEM_EVENT_FMP_REBOOT_IMMEDIATELY ) {
          // Intentionally left empty: no action required on immediate reboot status
        }
        break;

      case SMTC_MODEM_EVENT_UPLOAD_DONE:
        SMTC_HAL_TRACE_INFO("Event received: UPLOAD_DONE\n");
        break;

      case SMTC_MODEM_EVENT_STREAM_DONE:
        SMTC_HAL_TRACE_INFO("Event received: STREAM_DONE\n");
        break;

      case SMTC_MODEM_EVENT_MUTE:
        SMTC_HAL_TRACE_INFO("Event received: MUTE\n");
        break;

      case SMTC_MODEM_EVENT_DM_SET_CONF:
        SMTC_HAL_TRACE_INFO("Event received: DM_SET_CONF\n");
        break;

      case SMTC_MODEM_EVENT_RELAY_TX_MODE:
        SMTC_HAL_TRACE_INFO("Event received: RELAY_TX_MODE\n");
        break;

      case SMTC_MODEM_EVENT_RELAY_TX_DYNAMIC:
        SMTC_HAL_TRACE_INFO("Event received: RELAY_TX_DYNAMIC\n");
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

      case SMTC_MODEM_EVENT_RELAY_TX_SYNC:
        SMTC_HAL_TRACE_INFO("Event received: RELAY_TX_SYNC\n");
        break;

      case SMTC_MODEM_EVENT_TEST_MODE:
      {
        uint8_t status_test_mode = (uint8_t)current_event.event_data.test_mode_status.status;
#if MODEM_HAL_DBG_TRACE == MODEM_HAL_FEATURE_ON
        char* status_name[] = { "SMTC_MODEM_EVENT_TEST_MODE_ENDED", "SMTC_MODEM_EVENT_TEST_MODE_TX_COMPLETED",
                                "SMTC_MODEM_EVENT_TEST_MODE_TX_DONE", "SMTC_MODEM_EVENT_TEST_MODE_RX_DONE" };
        SMTC_HAL_TRACE_INFO("Event received: TEST_MODE :  %s\n", status_name[status_test_mode]);
#endif
        if ( status_test_mode == SMTC_MODEM_EVENT_TEST_MODE_RX_DONE ) {
          int16_t rssi;
          int16_t snr;
          uint8_t rx_payload_length;
          smtc_modem_test_get_last_rx_packets(&rssi, &snr, rx_payload, &rx_payload_length);
          SMTC_HAL_TRACE_ARRAY("rx_payload", rx_payload, rx_payload_length);
          SMTC_HAL_TRACE_PRINTF("rssi: %d, snr: %d\n", rssi, snr);
        }

        break;
      }

      case SMTC_MODEM_EVENT_REGIONAL_DUTY_CYCLE:
        SMTC_HAL_TRACE_INFO("Event received: DUTY_CYCLE\n");
        break;

      default:
        SMTC_HAL_TRACE_ERROR("Unknown event %u\n", current_event.event_type);
        break;
    }
  } while ( event_pending_count > 0 );
}

static void user_button_callback(void* context)
{
  ( void ) &context;    // Not used in the example - avoid warning
  static uint32_t last_press_timestamp_ms = 0;

  // Debounce the button press, avoid multiple triggers
  if ( ( int32_t ) (smtc_modem_hal_get_time_in_ms( ) - last_press_timestamp_ms) > 500 ) {
    last_press_timestamp_ms = smtc_modem_hal_get_time_in_ms( );
    user_button_is_press    = true;
    SMTC_HAL_TRACE_INFO("Button pressed\n");
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
  ASSERT_SMTC_MODEM_RC(smtc_modem_request_uplink(STACK_ID, port, false, buff, 4) );
  // Increment uplink counter
  uplink_counter++;
}

static void gps_time_to_date_string(uint32_t gps_time_s)
{
  // GPS epoch starts at January 6, 1980 00:00:00 UTC
  // Unix epoch starts at January 1, 1970 00:00:00 UTC
  // Difference between GPS and Unix epoch: 315964800 seconds (10 years + 2 leap days)
  const uint32_t GPS_TO_UNIX_OFFSET = 315964800;

  // Convert GPS time to Unix timestamp
  uint32_t unix_timestamp = gps_time_s + GPS_TO_UNIX_OFFSET;

  // Calculate days since Unix epoch
  uint32_t days = unix_timestamp / 86400; // 86400 seconds per day
  uint32_t seconds_in_day = unix_timestamp % 86400;

  // Calculate hours, minutes, seconds
  uint32_t hours = seconds_in_day / 3600;
  uint32_t minutes = (seconds_in_day % 3600) / 60;
  uint32_t seconds = seconds_in_day % 60;

  // Calculate year, month, day from Unix timestamp
  // (we already converted GPS time to Unix timestamp above)
  uint32_t year = 1970;  // Start from Unix epoch since we converted GPS->Unix
  uint32_t days_in_year;

  // Find the year
  while (days >= (days_in_year = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 366 : 365)) {
    days -= days_in_year;
    year++;
  }

  // Days in each month (non-leap year)
  static const uint8_t days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  // Find the month
  uint32_t month = 1;
  uint32_t days_in_current_month;

  while (month <= 12) {
    days_in_current_month = days_in_month[month - 1];

    // Check for leap year and February
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
      days_in_current_month = 29;
    }

    if (days < days_in_current_month) {
      break;
    }

    days -= days_in_current_month;
    month++;
  }

  uint32_t day = days + 1; // Convert to 1-based day

  // Format the date string as DD/MM/YY HH:MM:SS
  SMTC_HAL_TRACE_INFO("GPS time: %lu s -> Date: %02lu/%02lu/%02lu %02lu:%02lu:%02lu\n",
                      gps_time_s, day, month, year % 100, hours, minutes, seconds);
}

static void periodical_uplink_init(void)
{
  // Configure all the µC periph (clock, gpio, timer, ...)
  hal_mcu_init();

  // Configure Nucleo blue button as EXTI
  static hal_gpio_irq_t exti_button = {
    .port     = EXTI_BUTTON_PORT,
    .pin      = EXTI_BUTTON_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_NONE,
    .irq_mode = SL_GPIO_INTERRUPT_FALLING_EDGE,
    .context  = NULL,                         // context pass to the callback - not used in this example
    .callback = &user_button_callback,         // callback called when EXTI is triggered
  };
  hal_gpio_init_in(&exti_button);

  // Init the modem and use modem_event_callback as event callback, please note that the callback will be
  // called immediately after the first call to smtc_modem_run_engine because of the reset detection
  smtc_modem_init(&modem_event_callback);

  SMTC_HAL_TRACE_INFO("Periodical uplink example is starting \n");
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
