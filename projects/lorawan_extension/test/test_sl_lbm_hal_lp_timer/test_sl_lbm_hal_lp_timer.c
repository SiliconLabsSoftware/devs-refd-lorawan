/***************************************************************************//**
 * @file test_sl_lbm_hal_lp_timer.c
 * @brief Unit tests for LBM HAL Low Power Timer module
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Labs Inc.
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
#include "test_sl_lbm_hal_lp_timer.h"
#include "sl_interrupt_manager.h"
#include "smtc_hal_lp_timer.h"
#include "sl_hal_letimer.h"
#include "sl_sleeptimer.h"
#include "smtc_modem_hal.h"
#include <stdbool.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_TIMER_TOLERANCE_MS    5    // Acceptable timing tolerance in ms
#define TEST_SHORT_DELAY_MS       10    // Short timer delay for testing
#define TEST_MEDIUM_DELAY_MS      50    // Medium timer delay for testing
#define TEST_LONG_DELAY_MS        100   // Long timer delay for testing

#define TEST_TIMER_START_STOP_CONTEXT         (void*)0x12345678 // Context for timer start/stop tests
#define TEST_TIMER_IRQ_ENABLE_DISABLE_CONTEXT (void*)0x87654321 // Context for IRQ tests
#define TEST_TIMER_CALLBACK_CONTEXT           (void*)0x11223344 // Context for callback tests
#define TEST_TIMER_OVERFLOW_CONTEXT           (void*)0x22334455 // Context for overflow tests
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_timer_callback(void* context);
static void test_hal_lp_timer_initialization(void);
static void test_hal_lp_timer_start_stop(void);
static void test_hal_lp_timer_irq_enable_disable(void);
static void test_hal_lp_timer_callback_execution(void);
static void test_hal_lp_timer_multiple_starts(void);
static void test_hal_lp_timer_timing_accuracy(void);
static void test_hal_lp_timer_overflow(void);
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static volatile bool test_callback_called = false;
static volatile void* test_callback_context = NULL;
static volatile uint32_t test_callback_count = 0;
static volatile uint32_t test_interrupt_flags = 0;
static const hal_lp_timer_init_t test_lp_timer_init = SMTC_HAL_LP_TIMER_INIT_DEFAULT;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Run all LP timer HAL unit tests
 */
int sl_lbm_hal_run_lp_timer_test(void)
{
  // Reset Unity test framework
  UnityBegin(__FILE__);

  printf("\n=== Running LBM HAL LP Timer Tests ===\n");

  // Run each test function
  RUN_TEST(test_hal_lp_timer_initialization);
  RUN_TEST(test_hal_lp_timer_irq_enable_disable);
  RUN_TEST(test_hal_lp_timer_start_stop);
  RUN_TEST(test_hal_lp_timer_callback_execution);
  RUN_TEST(test_hal_lp_timer_multiple_starts);
  RUN_TEST(test_hal_lp_timer_timing_accuracy);
  RUN_TEST(test_hal_lp_timer_overflow);
  // Return the number of failed tests
  return UnityEnd();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test callback function for timer interrupt testing
 */
static void test_timer_callback(void* context)
{
  test_callback_called = true;
  test_callback_context = context;
  test_callback_count++;
  // Get the pending interrupts
  test_interrupt_flags = sl_hal_letimer_get_pending_interrupts(LETIMER(test_lp_timer_init.id));
}

/**
 * @brief Test LP timer initialization
 */
static void test_hal_lp_timer_initialization(void)
{
  // Verify timer initialization
  TEST_ASSERT_FALSE_MESSAGE(hal_lp_timer_init() == SL_STATUS_INVALID_INDEX,
                            "LP Timer index is invalid");
  TEST_ASSERT_FALSE_MESSAGE(hal_lp_timer_init() == SL_STATUS_NOT_AVAILABLE,
                            "LP Timer peripheral is not available");
  TEST_ASSERT_TRUE_MESSAGE(hal_lp_timer_init() == SL_STATUS_OK,
                           "LP Timer initialization failed");

  // Verify interrupt flags are cleared
  uint32_t pending_interrupts = sl_hal_letimer_get_pending_interrupts(LETIMER(test_lp_timer_init.id));
  TEST_ASSERT_EQUAL_UINT32(0, pending_interrupts);

  // Verify that LETIMER interrupts is enabled after initialization
  bool irq_disabled = sl_interrupt_manager_is_irq_disabled(test_lp_timer_init.irq_num);
  TEST_ASSERT_FALSE(irq_disabled);
}

/**
 * @brief Test timer interrupt enable and disable functionality
 */
static void test_hal_lp_timer_irq_enable_disable(void)
{
  // Reset test variables
  test_callback_called = false;
  test_callback_context = NULL;

  // Prepare timer IRQ structure
  hal_lp_timer_irq_t timer_irq = {
    .context = TEST_TIMER_IRQ_ENABLE_DISABLE_CONTEXT,
    .callback = test_timer_callback
  };

  // Test interrupt enable
  hal_lp_timer_irq_enable();
  TEST_ASSERT_FALSE_MESSAGE(sl_interrupt_manager_is_irq_disabled(LETIMER0_IRQn), "LETIMER interrupt should be enabled");

  // Start timer with IRQ enabled
  hal_lp_timer_start(TEST_SHORT_DELAY_MS, &timer_irq);

  // Wait for the timer to expire
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  // Verify that the COMP0 interrupt flag is set
  TEST_ASSERT_TRUE_MESSAGE(test_interrupt_flags & LETIMER_IF_COMP0, "COMP0 interrupt should be set after timer expiration");
  // Verify that the callback was called
  TEST_ASSERT_TRUE_MESSAGE(test_callback_called, "Timer callback was not called after timer expiration");
  // Verify that the callback context matches
  TEST_ASSERT_EQUAL_PTR_MESSAGE(timer_irq.context, test_callback_context, "Timer callback context does not match");
  // Stop timer after test finishes
  hal_lp_timer_stop();

  // Test interrupt disable
  // Reset test parameter
  test_callback_called = false;
  test_callback_context = NULL;
  test_interrupt_flags = 0;

  // Start timer with IRQ disabled
  hal_lp_timer_irq_disable();
  hal_lp_timer_start(TEST_SHORT_DELAY_MS, &timer_irq);
  TEST_ASSERT_TRUE_MESSAGE(sl_interrupt_manager_is_irq_disabled(LETIMER0_IRQn), "LETIMER interrupt should be disabled after IRQ disable");
  // Wait for the timer to expire
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);
  // Verify that the callback was not called
  TEST_ASSERT_FALSE_MESSAGE(test_callback_called, "Timer callback should not be called when IRQ is disabled");
  // Verify that the COMP0 interrupt flag is not set
  TEST_ASSERT_FALSE_MESSAGE(test_interrupt_flags & LETIMER_IF_COMP0, "COMP0 interrupt should not be set when IRQ is disabled");
  // Verify that the callback context is NULL
  TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, test_callback_context, "Timer callback context should be NULL when IRQ is disabled");
  // Stop timer after test finishes
  hal_lp_timer_stop();
}

/**
 * @brief Test timer start and stop functionality
 */
static void test_hal_lp_timer_start_stop(void)
{
  // Reset test variables
  test_callback_called = false;
  test_callback_context = NULL;
  hal_lp_timer_irq_enable();
  // Test timer start
  smtc_modem_hal_start_timer(TEST_SHORT_DELAY_MS, test_timer_callback, TEST_TIMER_START_STOP_CONTEXT);

  // Wait for the timer to expire (busy wait with safety timeout)
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);
  TEST_ASSERT_TRUE_MESSAGE(test_callback_called, "Timer callback was not called after start");

  // Verify that the callback was called with the correct context
  TEST_ASSERT_EQUAL_PTR_MESSAGE(TEST_TIMER_START_STOP_CONTEXT, test_callback_context, "Timer callback context does not match");
  // Stop timer after test finishes
  smtc_modem_hal_stop_timer();

  // Test timer stop
  test_callback_called = false; // Reset callback called flag
  test_callback_context = NULL; // Reset callback context

  smtc_modem_hal_start_timer(TEST_LONG_DELAY_MS, test_timer_callback, TEST_TIMER_START_STOP_CONTEXT);
  smtc_modem_hal_stop_timer();
  // Delay for a long period
  sl_sleeptimer_delay_millisecond(TEST_LONG_DELAY_MS + TEST_TIMER_TOLERANCE_MS);
  // Verify that call back is not called before timer is stopped
  TEST_ASSERT_FALSE_MESSAGE(test_callback_called, "Timer callback should not be called before timer is expired");
  TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, test_callback_context, "Timer callback context should be NULL before timer is expired");
}

/**
 * @brief Test timer callback execution
 */
static void test_hal_lp_timer_callback_execution(void)
{
  // Reset test variables
  test_callback_called = false;
  test_callback_context = NULL;
  test_callback_count = 0;

  hal_lp_timer_irq_enable();
  // Start timer with short delay
  smtc_modem_hal_start_timer(TEST_SHORT_DELAY_MS, test_timer_callback, TEST_TIMER_CALLBACK_CONTEXT);

  // Wait for timer to expire (busy wait with safety timeout)
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  // Verify callback was called
  TEST_ASSERT_TRUE_MESSAGE(test_callback_called, "Timer callback was not called");

  // Verify correct context was passed
  TEST_ASSERT_EQUAL_PTR(TEST_TIMER_CALLBACK_CONTEXT, test_callback_context);

  // Verify callback count
  TEST_ASSERT_EQUAL_UINT32(1, test_callback_count);

  // Stop timer after test finishes
  smtc_modem_hal_stop_timer();
}

/**
 * @brief Test multiple timer starts behavior
 */
static void test_hal_lp_timer_multiple_starts(void)
{
  // Reset test variables
  test_callback_called = false;
  test_callback_count = 0;

  hal_lp_timer_irq_enable();
  // Start first timer
  smtc_modem_hal_start_timer(TEST_LONG_DELAY_MS, test_timer_callback, NULL);

  // Start second timer (should override the first)
  smtc_modem_hal_start_timer(TEST_SHORT_DELAY_MS, test_timer_callback, NULL);

  // Wait for the shorter timer to expire
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);
  // Verify the second (shorter) timer fired, not the first one
  TEST_ASSERT_TRUE_MESSAGE(test_callback_called, "Timer callback was not called");

  // Verify that the callback was called only once
  test_callback_called = false; // Reset for next test
  // Stop any remaining timer activity
  smtc_modem_hal_stop_timer();
  // Wait for the longer timer to expire
  sl_sleeptimer_delay_millisecond(TEST_LONG_DELAY_MS - TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  // Verify that the callback was not called again
  TEST_ASSERT_FALSE_MESSAGE(test_callback_called, "Timer callback should not be called after stopping the timer");
  // Verify that the callback count is still 1
  TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, test_callback_count, "Timer callback count should be 1 after multiple starts");
}

/**
 * @brief Test timer timing accuracy
 */
static void test_hal_lp_timer_timing_accuracy(void)
{
  // Reset test variables
  test_callback_called = false;
  test_callback_count = 0;

  uint32_t timer_freq = hal_lp_timer_get_peripheral_freq();
  float ms_per_ticks = (float)timer_freq / 1000.0f;
  hal_lp_timer_irq_enable();
  // Start timer
  smtc_modem_hal_start_timer(TEST_MEDIUM_DELAY_MS, test_timer_callback, NULL);

  // Wait for timer to expire
  sl_sleeptimer_delay_millisecond(TEST_MEDIUM_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  // Verify timer fired
  TEST_ASSERT_TRUE_MESSAGE(test_callback_called, "Timer did not fire within expected timeframe");
  // Calculate elapsed time
  uint32_t end_counter = 0xFFFFFF - sl_hal_letimer_get_counter(LETIMER(test_lp_timer_init.id));
  // Convert elapsed_time to milliseconds
  int32_t accuracy_time = (uint32_t)(end_counter * ms_per_ticks) - (TEST_MEDIUM_DELAY_MS + TEST_TIMER_TOLERANCE_MS);
  TEST_ASSERT_TRUE_MESSAGE((accuracy_time < TEST_TIMER_TOLERANCE_MS || accuracy_time > -TEST_TIMER_TOLERANCE_MS),
                           "Timer did not fire within expected timing accuracy");

  // Stop timer after test finishes
  smtc_modem_hal_stop_timer();
}

static void test_hal_lp_timer_overflow(void)
{
  // Reset test variables
  test_callback_called = false;
  test_callback_context = NULL;

  uint32_t frequency = hal_lp_timer_get_peripheral_freq();
  float ms_per_ticks = (float)frequency / 1000.0f;
  hal_lp_timer_irq_enable();
  // Start 100ms timer
  smtc_modem_hal_start_timer(TEST_SHORT_DELAY_MS, test_timer_callback, TEST_TIMER_OVERFLOW_CONTEXT);
  // Delay until timer has been expired
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  TEST_ASSERT_TRUE(test_callback_called);
  TEST_ASSERT_EQUAL_PTR(TEST_TIMER_OVERFLOW_CONTEXT, test_callback_context);

  // Reset test variables
  test_callback_called = false;
  test_callback_context = NULL;
  test_interrupt_flags = 0;

  // Keep timer running and reduce counter to make test faster
  sl_hal_letimer_set_counter(LETIMER(test_lp_timer_init.id), (uint32_t)(TEST_LONG_DELAY_MS * ms_per_ticks));

  // Wait until timer is overflow
  sl_sleeptimer_delay_millisecond(TEST_LONG_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  // Verify that the timer overflowed
  TEST_ASSERT_FALSE_MESSAGE(test_callback_called, "Timer callback should not be called");
  TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, test_callback_context, "Timer callback context is not NULL");

  // Start counter after overflowed to verify result
  // Reset test variables
  test_callback_called = false;
  test_callback_context = NULL;
  test_interrupt_flags = 0;

  // Start 10ms timer
  smtc_modem_hal_start_timer(TEST_SHORT_DELAY_MS, test_timer_callback, TEST_TIMER_OVERFLOW_CONTEXT);

  // Delay until timer has been expired
  sl_sleeptimer_delay_millisecond(TEST_SHORT_DELAY_MS + TEST_TIMER_TOLERANCE_MS);

  TEST_ASSERT_TRUE(test_callback_called);
  TEST_ASSERT_EQUAL_PTR(TEST_TIMER_OVERFLOW_CONTEXT, test_callback_context);
}
