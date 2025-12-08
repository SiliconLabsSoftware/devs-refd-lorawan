/***************************************************************************//**
 * @brief Unit tests for LBM HAL GPIO
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
#include "test_sl_lbm_hal_gpio.h"
#include "unity.h"
#include "smtc_hal_gpio.h"
#include "em_gpio.h"
#include "sl_gpio.h"
#include "app_log.h"
#include <stdio.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_GPIO_PORT_A              gpioPortA
#define TEST_GPIO_PORT_B              gpioPortB
#define TEST_GPIO_PIN_0               0
#define TEST_GPIO_PIN_1               1
#define TEST_GPIO_PIN_2               2
#define TEST_GPIO_MAX_PIN             14
#define TEST_GPIO_PIN_INVALID         15

// GPIO test pins configuration
#define TEST_OUTPUT_PIN_PORT          TEST_GPIO_PORT_A
#define TEST_OUTPUT_PIN_NUM           TEST_GPIO_PIN_0
#define TEST_INPUT_PIN_PORT           TEST_GPIO_PORT_A
#define TEST_INPUT_PIN_NUM            TEST_GPIO_PIN_1

// External interrupt test configuration PB01 button
#define EXT_INT_BUTTON_PORT           TEST_GPIO_PORT_B
#define EXT_INT_BUTTON_PIN            TEST_GPIO_PIN_1
#define EXT_INT_TEST_TIMEOUT_MS       5000    // 5 second timeout for manual test

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_hal_gpio_init_out_basic(void);
static void test_hal_gpio_init_out_boundary_conditions(void);
static void test_hal_gpio_set_get_value_basic(void);
static void test_hal_gpio_set_get_value_boundary_conditions(void);

#if ENABLE_EXTERNAL_INTERRUPT_TEST
static void test_hal_gpio_external_interrupt_pb01(void);
static void sli_ext_int_callback(void *context);
#endif

// Helper functions
static void sli_gpio_cleanup_test_pins(void);
static void sli_gpio_setup_test_environment(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#if ENABLE_EXTERNAL_INTERRUPT_TEST
static volatile bool sli_ext_int_triggered = false;
static uint32_t sli_ext_int_count = 0;
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_gpio_test(void)
{
  UNITY_BEGIN();

  printf("\n=== Starting GPIO HAL Tests ===\n");

  // Setup test environment
  sli_gpio_setup_test_environment();

  // Test 1: Basic GPIO output initialization and functionality
  RUN_TEST(test_hal_gpio_init_out_basic);
  RUN_TEST(test_hal_gpio_init_out_boundary_conditions);

  // Test 2: GPIO set/get value operations
  RUN_TEST(test_hal_gpio_set_get_value_basic);
  RUN_TEST(test_hal_gpio_set_get_value_boundary_conditions);

#if ENABLE_EXTERNAL_INTERRUPT_TEST
  // Test 3: External interrupt test
  printf("\n--- Running External Interrupt Test (PB01 Button) ---\n");
  printf("Please press the button on PB01 within %d seconds...\n", EXT_INT_TEST_TIMEOUT_MS / 1000);
  RUN_TEST(test_hal_gpio_external_interrupt_pb01);
#else
  printf("\n--- External Interrupt Test DISABLED ---\n");
  printf("Set ENABLE_EXTERNAL_INTERRUPT_TEST to 1 to enable hardware test\n");
#endif

  // Cleanup
  sli_gpio_cleanup_test_pins();

  printf("=== GPIO HAL Tests Complete ===\n");

  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test basic GPIO output initialization
 */
static void test_hal_gpio_init_out_basic(void)
{
  // Test initialization with value 0
  hal_gpio_init_out(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 0);
  uint32_t value = hal_gpio_get_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM);
  TEST_ASSERT_EQUAL_UINT32(0, value);

  // Test initialization with value 1
  hal_gpio_init_out(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 1);
  value = hal_gpio_get_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM);
  TEST_ASSERT_EQUAL_UINT32(1, value);

  // Test initialization with non-zero value (should be treated as 1)
  hal_gpio_init_out(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 255);
  value = hal_gpio_get_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM);
  TEST_ASSERT_EQUAL_UINT32(1, value);

  TEST_PASS_MESSAGE("GPIO output initialization basic test passed");
}

/**
 * @brief Test GPIO output initialization boundary conditions
 */
static void test_hal_gpio_init_out_boundary_conditions(void)
{
  // Test with maximum valid pin number
  hal_gpio_init_out(TEST_GPIO_PORT_A, TEST_GPIO_MAX_PIN, 0);
  uint32_t value = hal_gpio_get_value(TEST_GPIO_PORT_A, TEST_GPIO_MAX_PIN);
  TEST_ASSERT_EQUAL_UINT32(0, value);

  TEST_PASS_MESSAGE("GPIO output initialization boundary conditions passed");
}

/**
 * @brief Test basic GPIO set/get value operations
 */
static void test_hal_gpio_set_get_value_basic(void)
{
  printf("Testing GPIO set/get value operations...\n");

  // Initialize pin as output
  hal_gpio_init_out(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 0);

  // Test setting value to 1
  hal_gpio_set_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 1);
  uint32_t value = hal_gpio_get_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM);
  TEST_ASSERT_EQUAL_UINT32(1, value);

  // Test setting value to 0
  hal_gpio_set_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 0);
  value = hal_gpio_get_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM);
  TEST_ASSERT_EQUAL_UINT32(0, value);

  // Test setting non-zero value (should be treated as 1)
  hal_gpio_set_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 42);
  value = hal_gpio_get_value(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM);
  TEST_ASSERT_EQUAL_UINT32(1, value);

  TEST_PASS_MESSAGE("GPIO set/get value basic test passed\n");
}

/**
 * @brief Test GPIO set/get value boundary conditions
 */
static void test_hal_gpio_set_get_value_boundary_conditions(void)
{
  // Test with maximum valid pin number
  hal_gpio_init_out(TEST_GPIO_PORT_A, TEST_GPIO_MAX_PIN, 0);
  hal_gpio_set_value(TEST_GPIO_PORT_A, TEST_GPIO_MAX_PIN, 1);
  uint32_t value = hal_gpio_get_value(TEST_GPIO_PORT_A, TEST_GPIO_MAX_PIN);
  TEST_ASSERT_EQUAL_UINT32(1, value);

  // Test with invalid pin number
  hal_gpio_set_value(TEST_GPIO_PORT_A, TEST_GPIO_PIN_INVALID, 1);
  value = hal_gpio_get_value(TEST_GPIO_PORT_A, TEST_GPIO_PIN_INVALID);
  TEST_ASSERT_EQUAL_UINT32(0, value);    // Should return 0 for invalid pins

  TEST_PASS_MESSAGE("GPIO set/get value boundary conditions test passed\n");
}

#if ENABLE_EXTERNAL_INTERRUPT_TEST
/**
 * @brief Test external interrupt functionality with PB01 button
 */
static void test_hal_gpio_external_interrupt_pb01(void)
{
  printf("Testing external interrupt with PB01 button...\n");
  printf("Press the button on PB01 to trigger interrupt...\n");

  // Reset interrupt flags
  sli_ext_int_triggered = false;
  sli_ext_int_count = 0;

  // Setup IRQ context for PB01 button
  hal_gpio_irq_t button_irq = {
    .port = EXT_INT_BUTTON_PORT,
    .pin = EXT_INT_BUTTON_PIN,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_UP,
    .irq_mode = SL_GPIO_INTERRUPT_RISING_EDGE,
    .context = &sli_ext_int_count,
    .callback = sli_ext_int_callback
  };

  // Initialize PB01 as input with pull-up and rising edge interrupt
  hal_gpio_init_in(&button_irq);

  // Enable GPIO interrupts
  hal_gpio_irq_enable();

  // Wait for button press
  uint32_t timeout_count = 0;
  const uint32_t max_timeout = EXT_INT_TEST_TIMEOUT_MS / 10;    // Check every 10ms

  while (!sli_ext_int_triggered && timeout_count < max_timeout) {
    // Simple delay loop (10ms approximation)
    for (volatile uint32_t i = 0; i < 100000; i++) {
      // Do nothing - just wait
    }
    timeout_count++;
  }

  if (sli_ext_int_triggered) {
    printf("External interrupt triggered successfully!\n");
    printf("Interrupt count: %lu\n", sli_ext_int_count);
    TEST_ASSERT_TRUE(sli_ext_int_triggered);
    TEST_ASSERT_GREATER_THAN_UINT32(0, sli_ext_int_count);
  } else {
    printf("No button press detected within timeout period\n");
    printf("This test requires manual interaction with PB01 button\n");
    TEST_IGNORE_MESSAGE("External interrupt test requires manual button press");
  }

  // Clean up
  hal_gpio_irq_detach(&button_irq);
  hal_gpio_irq_disable();
}

/**
 * @brief External interrupt callback function
 */
static void sli_ext_int_callback(void *context)
{
  volatile uint32_t *count = (volatile uint32_t *)context;
  if (count != NULL) {
    (*count)++;
  }
  sli_ext_int_triggered = true;

  // Clear the pending interrupt
  hal_gpio_clear_pending_irq(EXT_INT_BUTTON_PIN);

  printf("External interrupt callback triggered! Count: %lu\n",
         count ? *count : 0);
}
#endif

/**
 * @brief Setup test environment
 */
static void sli_gpio_setup_test_environment(void)
{
  // Enable GPIO clock
  hal_gpio_enable_clock();

  // Initialize test pins to known state
  hal_gpio_init_out(TEST_OUTPUT_PIN_PORT, TEST_OUTPUT_PIN_NUM, 0);

  // Setup input pin IRQ structure
  hal_gpio_irq_t input_irq = {
    .port = TEST_INPUT_PIN_PORT,
    .pin = TEST_INPUT_PIN_NUM,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_NONE,
    .irq_mode = SL_GPIO_INTERRUPT_NO_EDGE,
    .callback = NULL,
    .context = NULL
  };
  hal_gpio_init_in(&input_irq);
}

/**
 * @brief Cleanup test pins after testing
 */
static void sli_gpio_cleanup_test_pins(void)
{
  // Reset test pins to input mode without pull
  hal_gpio_irq_t output_irq = {
    .port = TEST_OUTPUT_PIN_PORT,
    .pin = TEST_OUTPUT_PIN_NUM,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_NONE,
    .irq_mode = SL_GPIO_INTERRUPT_NO_EDGE,
    .callback = NULL,
    .context = NULL
  };
  hal_gpio_init_in(&output_irq);

  hal_gpio_irq_t input_irq = {
    .port = TEST_INPUT_PIN_PORT,
    .pin = TEST_INPUT_PIN_NUM,
    .pull_mode = SMTC_HAL_GPIO_PULL_MODE_NONE,
    .irq_mode = SL_GPIO_INTERRUPT_NO_EDGE,
    .callback = NULL,
    .context = NULL
  };
  hal_gpio_init_in(&input_irq);

  // Disable GPIO interrupts
  hal_gpio_irq_disable();
}
