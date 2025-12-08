/***************************************************************************//**
 * @brief Unit tests for LBM HAL IADC
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
#include "unity.h"
#include "smtc_hal_iadc.h"
#include "em_iadc.h"
#include "em_cmu.h"
#include "app_log.h"
#include "sl_sleeptimer.h"
#include <stdio.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define TEST_IADC_TIMEOUT_MS                1000
#define ACCEPTABLE_VARIATION_VOLTAGE_MV     10
#define MANUAL_CHANGE_DELAY_MS              5000  // 5 seconds for manual hardware change

// Voltage test mode selection (set to one of the values below)
// 1 = CRITICAL (2.1V)
// 2 = NORMAL   (2.5V)
// 3 = FULL     (3.1V)
// 4 = EXTERNAL (3.2V)
#define TEST_VOLTAGE_MODE 4

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_hal_init_iadc0_basic(void);
static void test_hal_get_battery_level(void);
static void test_hal_iadc_initialization_verification(void);
static void test_hal_battery_level_critical(void);
static void test_hal_battery_level_normal(void);
static void test_hal_battery_level_full(void);
static void test_hal_battery_level_external_power(void);

// Helper functions
static void sli_iadc_setup_test_environment(void);
static void sli_iadc_cleanup_test_environment(void);
static bool sli_verify_iadc_is_initialized(void);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static bool test_iadc_initialized = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_iadc_test(void)
{
  UNITY_BEGIN();

  printf("\n=== Starting IADC HAL Tests ===\n");
  printf("IMPORTANT: Manual battery level tests require WPK CLI voltage control!\n");
  printf("Use WPK CLI commands to set VMCU voltage for battery level testing.\n");
  printf("Example: Use 'target voltage <voltage>' command to change voltage.\n\n");

  // Setup test environment
  sli_iadc_setup_test_environment();

  // Test 1: Basic IADC initialization
  RUN_TEST(test_hal_init_iadc0_basic);
  RUN_TEST(test_hal_iadc_initialization_verification);

  // Test 2: Battery level measurement test
  RUN_TEST(test_hal_get_battery_level);

  // Test 3: Conditional voltage-specific tests (enable ONE at a time)
  // Use 'target voltage <value>' command BEFORE running test

#if defined(TEST_VOLTAGE_MODE)
  switch (TEST_VOLTAGE_MODE) {
    case 1:
      printf("Testing CRITICAL voltage (2.1V)...\n");
      RUN_TEST(test_hal_battery_level_critical);
      break;
    case 2:
      printf("Testing NORMAL voltage (2.5V)...\n");
      RUN_TEST(test_hal_battery_level_normal);
      break;
    case 3:
      printf("Testing FULL voltage (3.1V)...\n");
      RUN_TEST(test_hal_battery_level_full);
      break;
    case 4:
      printf("Testing EXTERNAL POWER (3.2V)...\n");
      RUN_TEST(test_hal_battery_level_external_power);
      break;
    default:
      printf("Invalid TEST_VOLTAGE_MODE value!\n");
      break;
  }
#else
  printf("No voltage test selected. Define TEST_VOLTAGE_MODE to run a test.\n");
#endif

  // Cleanup
  sli_iadc_cleanup_test_environment();

  printf("=== IADC HAL Tests Complete ===\n");

  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test basic IADC initialization
 */
static void test_hal_init_iadc0_basic(void)
{
  printf("Testing basic IADC initialization...\n");

  // Initialize IADC with AVDD measurement configuration
  hal_init_iadc0(iadcPosInputAvdd, iadcNegInputGnd);

  // Verify IADC is enabled and configured
  TEST_ASSERT_TRUE_MESSAGE(sli_verify_iadc_is_initialized(),
                           "IADC should be properly initialized");

  test_iadc_initialized = true;
  TEST_PASS_MESSAGE("Basic IADC initialization test passed");
}

/**
 * @brief Test battery level measurement functionality
 */
static void test_hal_get_battery_level(void)
{
  printf("Testing battery level measurement functionality...\n");

  // Test multiple readings for consistency
  uint8_t reading1 = hal_get_battery_level();
  uint8_t reading2 = hal_get_battery_level();
  uint8_t reading3 = hal_get_battery_level();

  // All readings should be valid (not error)
  TEST_ASSERT_NOT_EQUAL_MESSAGE(SMTC_HAL_BATTERY_ERROR, reading1,
                                "First battery reading should not return error");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(SMTC_HAL_BATTERY_ERROR, reading2,
                                "Second battery reading should not return error");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(SMTC_HAL_BATTERY_ERROR, reading3,
                                "Third battery reading should not return error");

  printf("Battery readings: %d, %d, %d\n", reading1, reading2, reading3);

  // Verify readings are in valid range or special codes
  for (int i = 0; i < 3; i++) {
    uint8_t reading = (i == 0) ? reading1 : (i == 1) ? reading2 : reading3;

    TEST_ASSERT_TRUE_MESSAGE(
      (reading >= SMTC_HAL_BATTERY_CRITICAL && reading <= SMTC_HAL_BATTERY_FULL)
      || reading == SMTC_HAL_BATTERY_FULL
      || reading == SMTC_HAL_EXTERNAL_POWER_SUPPLY
      || reading == SMTC_HAL_BATTERY_CRITICAL,
      "Battery level should be in valid range (1-254) or special code (0, 254, 1)");
  }

  // Test reading consistency - readings should be reasonably close
  uint8_t max_reading = (reading1 > reading2) ? reading1 : reading2;
  max_reading = (max_reading > reading3) ? max_reading : reading3;

  uint8_t min_reading = (reading1 < reading2) ? reading1 : reading2;
  min_reading = (min_reading < reading3) ? min_reading : reading3;

  // Allow some variation but not too much
  if (max_reading < SMTC_HAL_BATTERY_FULL && min_reading > SMTC_HAL_BATTERY_CRITICAL) {
    TEST_ASSERT_TRUE_MESSAGE(
      (max_reading - min_reading) <= ACCEPTABLE_VARIATION_VOLTAGE_MV,
      "Battery readings should be reasonably consistent (within 10 levels)");
  }

  TEST_PASS_MESSAGE("Battery level measurement functionality test passed");
}

/**
 * @brief Test IADC initialization verification
 */
static void test_hal_iadc_initialization_verification(void)
{
  printf("Testing IADC initialization verification...\n");

  // Verify IADC is working by testing battery level function
  uint8_t battery_level = hal_get_battery_level();
  TEST_ASSERT_NOT_EQUAL_MESSAGE(SMTC_HAL_BATTERY_ERROR, battery_level,
                                "IADC should be properly initialized and working");

  TEST_PASS_MESSAGE("IADC initialization verification test passed");
}

// -----------------------------------------------------------------------------
//                          Helper Function Definitions
// -----------------------------------------------------------------------------

static void sli_iadc_setup_test_environment(void)
{
  printf("Setting up IADC test environment...\n");
  test_iadc_initialized = false;
}

static void sli_iadc_cleanup_test_environment(void)
{
  printf("Cleaning up IADC test environment...\n");
  test_iadc_initialized = false;
}

static bool sli_verify_iadc_is_initialized(void)
{
  uint8_t test_result = hal_get_battery_level();
  return (test_result != SMTC_HAL_BATTERY_ERROR);
}

/**
 * @brief Test battery level at critical voltage (2.1V)
 */
static void test_hal_battery_level_critical(void)
{
  printf("Testing battery level at critical voltage (2.1V)...\n");
  printf("Use WPK CLI: 'target voltage 2.1' to set VMCU voltage to 2.1V\n");

  if (!test_iadc_initialized) {
    hal_init_iadc0(iadcPosInputAvdd, iadcNegInputGnd);
    test_iadc_initialized = true;
  }

  uint8_t level_critical = hal_get_battery_level();
  printf("Battery level at 2.1V: %d (expected: %d)\n", level_critical, SMTC_HAL_BATTERY_CRITICAL);

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_HAL_BATTERY_CRITICAL, level_critical,
                            "Should detect critical battery level at 2.1V");

  TEST_PASS_MESSAGE("Critical battery level test passed");
}

/**
 * @brief Test battery level at normal voltage (2.5V)
 */
static void test_hal_battery_level_normal(void)
{
  printf("Testing battery level at normal voltage (2.5V)...\n");
  printf("Use WPK CLI: 'target voltage 2.5' to set VMCU voltage to 2.5V\n");

  if (!test_iadc_initialized) {
    hal_init_iadc0(iadcPosInputAvdd, iadcNegInputGnd);
    test_iadc_initialized = true;
  }

  uint8_t level_normal = hal_get_battery_level();
  printf("Battery level at 2.5V: %d (expected: 2-253 range)\n", level_normal);

  TEST_ASSERT_TRUE_MESSAGE(level_normal >= 2 && level_normal <= 253,
                           "Should detect normal battery level at 2.5V (range 2-253)");

  TEST_PASS_MESSAGE("Normal battery level test passed");
}

/**
 * @brief Test battery level at full voltage (3.1V)
 */
static void test_hal_battery_level_full(void)
{
  printf("Testing battery level at full voltage (3.1V)...\n");
  printf("Use WPK CLI: 'target voltage set 3.1' to set VMCU voltage to 3.1V\n");

  if (!test_iadc_initialized) {
    hal_init_iadc0(iadcPosInputAvdd, iadcNegInputGnd);
    test_iadc_initialized = true;
  }

  uint8_t level_full = hal_get_battery_level();
  printf("Battery level at 3.1V: %d (expected: %d)\n", level_full, SMTC_HAL_BATTERY_FULL);

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_HAL_BATTERY_FULL, level_full,
                            "Should detect full battery level at 3.1V");

  TEST_PASS_MESSAGE("Full battery level test passed");
}

/**
 * @brief Test battery level at external power voltage (3.2V)
 */
static void test_hal_battery_level_external_power(void)
{
  printf("Testing battery level at external power voltage (3.2V)...\n");
  printf("Use WPK CLI: 'target voltage 3.2' to set VMCU voltage to 3.2V\n");

  if (!test_iadc_initialized) {
    hal_init_iadc0(iadcPosInputAvdd, iadcNegInputGnd);
    test_iadc_initialized = true;
  }

  uint8_t level_external = hal_get_battery_level();
  printf("Battery level at 3.2V: %d (expected: %d)\n", level_external, SMTC_HAL_EXTERNAL_POWER_SUPPLY);

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_HAL_EXTERNAL_POWER_SUPPLY, level_external,
                            "Should detect external power supply at 3.2V");

  TEST_PASS_MESSAGE("External power level test passed");
}
