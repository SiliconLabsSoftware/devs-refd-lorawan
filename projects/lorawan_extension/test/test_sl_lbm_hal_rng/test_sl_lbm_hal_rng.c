/***************************************************************************//**
 * @brief Unit tests for LBM HAL RNG
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
#include "test_sl_lbm_hal_rng.h"
#include "unity.h"
#include "smtc_hal_rng.h"
#include "psa/crypto.h"
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_RNG_ITERATIONS           100    // Number of iterations for statistical tests
#define TEST_RNG_MIN_VALUE            10     // Test minimum range value
#define TEST_RNG_MAX_VALUE            100    // Test maximum range value
#define TEST_RNG_TOLERANCE_PERCENT    20     // Tolerance percentage for distribution tests

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_hal_rng_initialization(void);
static void test_hal_rng_get_random_basic(void);
static void test_hal_rng_get_random_uniqueness(void);
static void test_hal_rng_get_random_in_range_basic(void);
static void test_hal_rng_get_random_in_range_bounds(void);
static void test_hal_rng_get_random_in_range_reversed_params(void);
static void test_hal_rng_get_random_in_range_same_values(void);
static void test_hal_rng_multiple_calls_stability(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int sl_lbm_hal_run_rng_test(void)
{
  UNITY_BEGIN();

  // Test 1: Initialize RNG and verify PSA crypto initialization
  RUN_TEST(test_hal_rng_initialization);

  // Test 2: Test basic random number generation
  RUN_TEST(test_hal_rng_get_random_basic);

  // Test 3: Test random number uniqueness
  RUN_TEST(test_hal_rng_get_random_uniqueness);

  // Test 4: Test basic range functionality
  RUN_TEST(test_hal_rng_get_random_in_range_basic);

  // Test 5: Test range bounds validation
  RUN_TEST(test_hal_rng_get_random_in_range_bounds);

  // Test 6: Test reversed parameter handling
  RUN_TEST(test_hal_rng_get_random_in_range_reversed_params);

  // Test 7: Test same values parameter handling
  RUN_TEST(test_hal_rng_get_random_in_range_same_values);

  // Test 8: Test multiple calls stability
  RUN_TEST(test_hal_rng_multiple_calls_stability);

  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test RNG initialization
 * This test verifies that the RNG initializes without errors and PSA crypto is ready
 */
static void test_hal_rng_initialization(void)
{
  hal_rng_init();

  // Verify that PSA crypto is initialized by checking if we can generate random data
  uint8_t test_buffer[4];
  psa_status_t status = psa_generate_random(test_buffer, sizeof(test_buffer));

  TEST_ASSERT_EQUAL_MESSAGE(PSA_SUCCESS, status, "PSA crypto should be initialized and able to generate random data");

  TEST_PASS_MESSAGE("RNG initialization completed successfully");
}

/**
 * @brief Test basic random number generation
 * This test verifies that random numbers are generated without errors
 */
static void test_hal_rng_get_random_basic(void)
{
  uint32_t random_value1 = 0;
  random_value1 = hal_rng_get_random();
  uint32_t random_value2 = 0;
  random_value2 = hal_rng_get_random();

  // Verify that actual random values have been generated (not zero)
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, random_value1, "First random value should not be zero");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, random_value2, "Second random value should not be zero");

  char msg[128];
  snprintf(msg, sizeof(msg), "Generated values: 0x%08lX, 0x%08lX", random_value1, random_value2);
  TEST_MESSAGE(msg);

  TEST_PASS_MESSAGE("Basic random number generation test passed - values are non-zero");
}

/**
 * @brief Test random number uniqueness
 * This test verifies that consecutive random numbers are different (statistical test)
 */
static void test_hal_rng_get_random_uniqueness(void)
{
  uint32_t random_values[10];
  int duplicate_count = 0;

  // Generate multiple random values
  for (int i = 0; i < 10; i++) {
    random_values[i] = hal_rng_get_random();
  }

  // Check for duplicates
  for (int i = 0; i < 10; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (random_values[i] == random_values[j]) {
        duplicate_count++;
      }
    }
  }

  // With a good RNG, duplicates should be extremely rare
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(1, duplicate_count,
                                    "Should have very few (ideally 0) duplicates in 10 random numbers");

  TEST_PASS_MESSAGE("Random number uniqueness test passed");
}

/**
 * @brief Test basic range functionality
 * This test verifies that random numbers are generated within the specified range
 */
static void test_hal_rng_get_random_in_range_basic(void)
{
  uint32_t min_val = TEST_RNG_MIN_VALUE;
  uint32_t max_val = TEST_RNG_MAX_VALUE;

  for (int i = 0; i < 20; i++) {
    uint32_t random_value = hal_rng_get_random_in_range(min_val, max_val);

    TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(min_val, random_value,
                                         "Random value should be >= minimum value");
    TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(max_val, random_value,
                                      "Random value should be <= maximum value");
  }

  TEST_PASS_MESSAGE("Basic range functionality test passed");
}

/**
 * @brief Test range bounds validation
 * This test verifies that the function handles edge cases correctly
 */
static void test_hal_rng_get_random_in_range_bounds(void)
{
  // Test very small range
  uint32_t result_small = hal_rng_get_random_in_range(1, 2);
  TEST_ASSERT_MESSAGE((result_small == 1 || result_small == 2),
                      "Small range should return valid value");

  // Test larger range
  uint32_t result_large = hal_rng_get_random_in_range(1, 1000);
  TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(1, result_large, "Large range min bound");
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(1000, result_large, "Large range max bound");

  TEST_PASS_MESSAGE("Range bounds validation test passed");
}

/**
 * @brief Test reversed parameter handling
 * This test verifies that the function works correctly when val1 > val2
 */
static void test_hal_rng_get_random_in_range_reversed_params(void)
{
  uint32_t val1 = TEST_RNG_MAX_VALUE;  // 100
  uint32_t val2 = TEST_RNG_MIN_VALUE;  // 10

  for (int i = 0; i < 10; i++) {
    uint32_t random_value = hal_rng_get_random_in_range(val1, val2);

    // Should still be within the range regardless of parameter order
    TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(TEST_RNG_MIN_VALUE, random_value,
                                         "Random value should be >= smaller value (reversed params)");
    TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(TEST_RNG_MAX_VALUE, random_value,
                                      "Random value should be <= larger value (reversed params)");
  }

  TEST_PASS_MESSAGE("Reversed parameter handling test passed");
}

/**
 * @brief Test same values parameter handling
 * This test verifies edge case when both parameters are identical (min == max)
 * The function should always return that exact value
 */
static void test_hal_rng_get_random_in_range_same_values(void)
{
  uint32_t test_value = 42;

  for (int i = 0; i < 5; i++) {
    uint32_t result = hal_rng_get_random_in_range(test_value, test_value);
    TEST_ASSERT_EQUAL_MESSAGE(test_value, result,
                              "When both parameters are same, should return that value");
  }

  TEST_PASS_MESSAGE("Same values parameter handling test passed");
}

/**
 * @brief Test multiple calls stability
 * This test verifies that the RNG remains stable after many calls
 */
static void test_hal_rng_multiple_calls_stability(void)
{
  uint32_t error_count = 0;

  // Make many calls and verify they complete without issues
  for (int i = 0; i < TEST_RNG_ITERATIONS; i++) {
    uint32_t range_val = hal_rng_get_random_in_range(1, 100);

    // Basic sanity checks
    if (range_val < 1 || range_val > 100) {
      error_count++;
    }
  }

  TEST_ASSERT_EQUAL_MESSAGE(0, error_count,
                            "All calls should complete successfully with valid results");

  TEST_PASS_MESSAGE("Multiple calls stability test passed");
}
