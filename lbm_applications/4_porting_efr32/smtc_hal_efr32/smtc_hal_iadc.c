/***************************************************************************//**
 * @file hal_iadc.c
 * @brief The LBM IADC Modem Hardware Abstraction Layer Source File
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
#include "smtc_hal_iadc.h"
#include "em_cmu.h"
#include "sl_sleeptimer.h"
#include "app_log.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// Set CLK_ADC to 100kHz (this corresponds to a sample rate of 10ksps)
#define CLK_SRC_ADC_FREQ                10000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ                    100       // CLK_ADC

#define VOLTAGE_SAMPLES_COUNT           10
#define VOLTAGE_STABILITY_THRESHOLD_MV  20

#define IADC_REFERENCE_VOLTAGE_MV       1200
#define IADC_AVDD_ATTENUATION_FACTOR    4
#define IADC_MAX_DIGITAL_VALUE          4095

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

static uint16_t hal_get_stable_voltage_mv(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void hal_init_iadc0(IADC_PosInput_t posInput, IADC_NegInput_t negInput)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  CMU_ClockEnable(cmuClock_IADC0, true);

  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);

  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use internal 1.2V bandgap as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                     CLK_ADC_FREQ, 0, iadcCfgModeNormal, init.srcClkPrescale);

  initSingleInput.posInput = posInput;
  initSingleInput.negInput = negInput;

  IADC_init(IADC0, &init, &initAllConfigs);
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);
}

uint8_t hal_get_battery_level(void)
{
  uint16_t voltage_mv = hal_get_stable_voltage_mv();
  if (voltage_mv == 0) {
    return SMTC_HAL_BATTERY_ERROR;
  }
  app_log_debug("Voltage: %d mV\n", voltage_mv);

  if (voltage_mv > EXTERNAL_POWER_MV) {
    app_log_debug("External power detected (voltage > %d mV)\n", EXTERNAL_POWER_MV);
    return SMTC_HAL_EXTERNAL_POWER_SUPPLY;
  }
  if (voltage_mv < BATTERY_MIN_MV) {
    return SMTC_HAL_BATTERY_CRITICAL;
  }
  if (voltage_mv >= BATTERY_MAX_MV) {
    return SMTC_HAL_BATTERY_FULL;
  }

  uint32_t battery_range = BATTERY_MAX_MV - BATTERY_MIN_MV;
  uint32_t voltage_offset = voltage_mv - BATTERY_MIN_MV;
  uint8_t battery_level = 1 + ((voltage_offset * 253) / battery_range);

  return battery_level;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static uint16_t hal_get_stable_voltage_mv(void)
{
  uint32_t voltage_sum = 0;
  uint16_t voltage_min = 0xFFFF;
  uint16_t voltage_max = 0;
  uint16_t current_voltage = 0;

  // Take multiple samples
  for (int i = 0; i < VOLTAGE_SAMPLES_COUNT; i++) {
    IADC_command(IADC0, iadcCmdStartSingle);

    // Wait for conversion
    uint8_t wait_count = 0;
    while (wait_count < 10) {
      if (IADC_getInt(IADC0) & IADC_IF_SINGLEDONE) {
        break;
      }
      sl_sleeptimer_delay_millisecond(IADC_SLEEP_INTERVAL_MS);
      wait_count++;
    }
    if (wait_count >= 10) {
      app_log_debug("IADC single conversion timeout\n");
      return 0;
    }

    IADC_clearInt(IADC0, IADC_IF_SINGLEDONE);
    IADC_Result_t result = IADC_readSingleResult(IADC0);
    if (result.data == 0) {
      app_log_debug("IADC read single result failed\n");
      return 0;
    }

    current_voltage = (result.data) * IADC_AVDD_ATTENUATION_FACTOR
                      * IADC_REFERENCE_VOLTAGE_MV / IADC_MAX_DIGITAL_VALUE;
    voltage_sum += current_voltage;

    if (current_voltage < voltage_min) {
      voltage_min = current_voltage;
    }
    if (current_voltage > voltage_max) {
      voltage_max = current_voltage;
    }

    sl_sleeptimer_delay_millisecond(IADC_SLEEP_INTERVAL_MS);
  }

  // Check if readings are stable
  if ((voltage_max - voltage_min) > VOLTAGE_STABILITY_THRESHOLD_MV) {
    app_log_debug("Voltage readings unstable: %d mV spread\n",
                  voltage_max - voltage_min);
    return 0;
  }

  // Return average
  return (uint16_t) (voltage_sum / VOLTAGE_SAMPLES_COUNT);
}
