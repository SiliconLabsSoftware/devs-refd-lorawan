/***************************************************************************//**
 * @file test_sl_lbm_radio_hal_lp_timer.c
 * @brief Unit tests for LBM Radio HAL Low Power Timer module
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
#define UNIT_TESTING_MODE  // Enable special testing mode in HAL

#include "unity.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "test_sl_lbm_radio_hal.h"
#include "sx126x_hal.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_CONTEXT_DUMMY    ((void*)0x12345678)
#define TEST_BUFFER_SIZE      16

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint8_t test_command_buffer[TEST_BUFFER_SIZE];
static uint8_t test_data_buffer[TEST_BUFFER_SIZE];

void setUp(void)
{
  // Initialize test buffers with known patterns
  memset(test_command_buffer, 0xAA, TEST_BUFFER_SIZE);
  memset(test_data_buffer, 0x55, TEST_BUFFER_SIZE);
}

void tearDown(void)
{
  // Clean up after each test
  // Nothing specific needed for these tests
}

void test_sx126x_hal_write_ValidParams_ShouldReturnOK(void)
{
  // Prepare test command and data
  test_command_buffer[0] = 0x80;    // Generic command
  test_command_buffer[1] = 0x00;
  test_data_buffer[0] = 0x01;
  test_data_buffer[1] = 0x02;
  test_data_buffer[2] = 0x03;

  // Test the function
  sx126x_hal_status_t result = sx126x_hal_write(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 2,
    test_data_buffer, 3
    );

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_write_ZeroCommandLength_ShouldReturnOK(void)
{
  sx126x_hal_status_t result = sx126x_hal_write(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 0,
    test_data_buffer, 3
    );

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_write_SleepCommand_ShouldSetSleepMode(void)
{
  // Prepare sleep command - this should set radio to sleep mode internally
  test_command_buffer[0] = 0x84;    // SX126x_SET_SLEEP opcode
  test_command_buffer[1] = 0x00;

  sx126x_hal_status_t result = sx126x_hal_write(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 2,
    NULL, 0
    );

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}
void test_sx126x_hal_read_ValidParams_ShouldReturnOK(void)
{
  // Prepare test command
  test_command_buffer[0] = 0x1D;    // Read register command
  test_command_buffer[1] = 0x00;
  test_command_buffer[2] = 0x00;

  // Clear data buffer
  memset(test_data_buffer, 0x00, TEST_BUFFER_SIZE);

  sx126x_hal_status_t result = sx126x_hal_read(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 3,
    test_data_buffer, 4
    );

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_read_ZeroDataLength_ShouldReturnOK(void)
{
  test_command_buffer[0] = 0x1D;
  test_command_buffer[1] = 0x00;
  test_command_buffer[2] = 0x00;

  sx126x_hal_status_t result = sx126x_hal_read(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 3,
    test_data_buffer, 0
    );

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_reset_ValidContext_ShouldReturnOK(void)
{
  sx126x_hal_status_t result = sx126x_hal_reset(TEST_CONTEXT_DUMMY);

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_wakeup_ValidContext_ShouldReturnOK(void)
{
  sx126x_hal_status_t result = sx126x_hal_wakeup(TEST_CONTEXT_DUMMY);

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_write_WithRealHardware_ShouldCommunicate(void)
{
  printf("\n[HW TEST] Testing sx126x_hal_write with real hardware...\n");

  // Test with NOP command (should always work)
  test_command_buffer[0] = 0x00;    // NOP command

  sx126x_hal_status_t result = sx126x_hal_write(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 1,
    NULL, 0
    );

  printf("[HW TEST] NOP command result: %s\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED");

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_write_RealSleepCommand_ShouldPutRadioToSleep(void)
{
  printf("\n[HW TEST] Testing sx126x_hal_write with sleep command...\n");

  // Send SET_SLEEP command with configuration
  test_command_buffer[0] = 0x84;    // SET_SLEEP
  test_command_buffer[1] = 0x00;    // Sleep config (cold start, no retention)

  sx126x_hal_status_t result = sx126x_hal_write(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 2,
    NULL, 0
    );

  printf("[HW TEST] Sleep command result: %s\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED");

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);

  // Small delay to allow radio to enter sleep
  for (volatile int i = 0; i < 100000; i++) {
    ;
  }
}

void test_sx126x_hal_read_WithRealHardware_ShouldReceiveData(void)
{
  printf("\n[HW TEST] Testing sx126x_hal_read with real hardware...\n");

  // Read status command
  test_command_buffer[0] = 0xC0;    // GET_STATUS

  memset(test_data_buffer, 0x00, TEST_BUFFER_SIZE);

  sx126x_hal_status_t result = sx126x_hal_read(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 1,
    test_data_buffer, 1
    );

  printf("[HW TEST] Status read result: %s\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED");
  printf("[HW TEST] Status value: 0x%02X\n", test_data_buffer[0]);

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);

  // Status should have valid bits set (not 0x00 or 0xFF typically)
  TEST_ASSERT_NOT_EQUAL(0x00, test_data_buffer[0]);
  TEST_ASSERT_NOT_EQUAL(0xFF, test_data_buffer[0]);
}

void test_sx126x_hal_read_ReadRadioVersion_ShouldReturnValidData(void)
{
  printf("\n[HW TEST] Testing sx126x_hal_read radio version...\n");

  // Read version info (register 0x0320)
  test_command_buffer[0] = 0x1D;    // READ_REGISTER
  test_command_buffer[1] = 0x03;    // Address MSB
  test_command_buffer[2] = 0x20;    // Address LSB
  test_command_buffer[3] = 0x00;    // NOP

  memset(test_data_buffer, 0x00, TEST_BUFFER_SIZE);

  sx126x_hal_status_t result = sx126x_hal_read(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 4,
    test_data_buffer, 1
    );

  printf("[HW TEST] Version read result: %s\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED");
  printf("[HW TEST] Version register: 0x%02X\n", test_data_buffer[0]);

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);

  // Version should be a reasonable value (SX126x typically 0x22 or similar)
  TEST_ASSERT_NOT_EQUAL(0x00, test_data_buffer[0]);
  TEST_ASSERT_NOT_EQUAL(0xFF, test_data_buffer[0]);
}

void test_sx126x_hal_reset_WithRealHardware_ShouldResetRadio(void)
{
  printf("\n[HW TEST] Testing sx126x_hal_reset with real hardware...\n");

  sx126x_hal_status_t result = sx126x_hal_reset(TEST_CONTEXT_DUMMY);

  printf("[HW TEST] Reset result: %s\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED");

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);

  // Wait for radio to complete reset sequence
  for (volatile int i = 0; i < 500000; i++) {
    ;
  }

  // Verify radio is responsive after reset by reading status
  test_command_buffer[0] = 0xC0;    // GET_STATUS
  memset(test_data_buffer, 0x00, TEST_BUFFER_SIZE);

  result = sx126x_hal_read(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 1,
    test_data_buffer, 1
    );

  printf("[HW TEST] Post-reset status read: %s (0x%02X)\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED",
         test_data_buffer[0]);

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

void test_sx126x_hal_wakeup_WithRealHardware_ShouldWakeRadio(void)
{
  printf("\n[HW TEST] Testing sx126x_hal_wakeup with real hardware...\n");

  // First put radio to sleep
  test_command_buffer[0] = 0x84;    // SET_SLEEP
  test_command_buffer[1] = 0x00;    // Sleep config

  sx126x_hal_status_t result = sx126x_hal_write(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 2,
    NULL, 0
    );

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);

  // Wait for sleep
  for (volatile int i = 0; i < 100000; i++) {
    ;
  }

  // Now test wakeup
  result = sx126x_hal_wakeup(TEST_CONTEXT_DUMMY);

  printf("[HW TEST] Wakeup result: %s\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED");

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);

  // Verify radio is awake by reading status
  test_command_buffer[0] = 0xC0;    // GET_STATUS
  memset(test_data_buffer, 0x00, TEST_BUFFER_SIZE);

  result = sx126x_hal_read(
    TEST_CONTEXT_DUMMY,
    test_command_buffer, 1,
    test_data_buffer, 1
    );

  printf("[HW TEST] Post-wakeup status read: %s (0x%02X)\n",
         (result == SX126X_HAL_STATUS_OK) ? "OK" : "FAILED",
         test_data_buffer[0]);

  TEST_ASSERT_EQUAL(SX126X_HAL_STATUS_OK, result);
}

/* --- EOF ------------------------------------------------------------------ */
