/***************************************************************************//**
 * @brief Unit tests for SMTC Modem Crypto functions
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
#include "test_smtc_modem_crypto.h"
#include "unity.h"
#include "smtc_modem_crypto.h"
#include "smtc_secure_element.h"
#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define TEST_STACK_ID                    0
#define TEST_PAYLOAD_SIZE               64
#define TEST_SMALL_PAYLOAD_SIZE         16
#define TEST_MAX_PAYLOAD_SIZE           64
#define TEST_FRAME_COUNTER        0x12345678
#define TEST_DEVICE_ADDRESS       0xABCDEF01
#define TEST_MULTICAST_ADDRESS    0x12345678
#define TEST_DIR_UPLINK                  0
#define TEST_DIR_DOWNLINK                1
#define TEST_NONCE_SIZE                 14
#define TEST_RAND_SIZE                  16
#define TEST_MIC_SIZE                    4

// Test vectors and expected values
#define TEST_KEY_SIZE                   16
#define TEST_ENCRYPT_BUFF_SIZE          16
#define LBM_USE_HW_CRYPTO               1

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void test_smtc_modem_crypto_payload_encrypt_basic(void);
static void test_smtc_modem_crypto_payload_encrypt_decrypt_roundtrip(void);
static void test_smtc_modem_crypto_payload_encrypt_different_sizes(void);
static void test_smtc_modem_crypto_payload_decrypt_basic(void);
static void test_smtc_modem_crypto_compute_join_mic_basic(void);
static void test_smtc_modem_crypto_process_join_accept_basic(void);
static void test_smtc_modem_crypto_derive_skeys_basic(void);
static void test_smtc_modem_crypto_verify_mic_basic(void);
static void test_smtc_modem_crypto_verify_mic_invalid_mic(void);
static void test_smtc_modem_crypto_verify_mic_oversized_buffer(void);
static void test_smtc_modem_crypto_compute_and_add_mic_basic(void);
static void test_smtc_modem_crypto_set_key_basic(void);
static void test_smtc_modem_crypto_set_key_app_key_derivation(void);
static void test_smtc_modem_crypto_derive_multicast_session_keys_basic(void);
static void test_smtc_modem_crypto_derive_multicast_session_keys_invalid_params(void);
static void test_smtc_modem_crypto_derive_relay_session_keys_basic(void);
static void test_smtc_modem_crypto_get_class_b_rand_basic(void);
static void test_smtc_modem_crypto_service_encrypt_basic(void);
static void test_smtc_modem_crypto_service_encrypt_different_sizes(void);

// Helper functions
static void setup_test_key(smtc_se_key_identifier_t key_id);
static void create_test_payload(uint8_t* payload, uint16_t size);
static void print_hex_buffer(const char* prefix, const uint8_t* buffer, uint16_t size);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint8_t test_key[TEST_KEY_SIZE] = {
  0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
  0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

static uint8_t test_join_nonce[LORAWAN_JOIN_NONCE_SIZE] = { 0x12, 0x34, 0x56 };
static uint8_t test_net_id[LORAWAN_NET_ID_SIZE] = { 0x78, 0x9A, 0xBC };
static uint16_t test_dev_nonce = 0xDEF0;
#ifdef LBM_USE_HW_CRYPTO
static uint8_t test_encrypt_basic_buf[TEST_PAYLOAD_SIZE] = { 0x75, 0x2E, 0x8D, 0xD0, 0x43, 0xE2, 0x5F, 0xBA, 0xF3, 0x92, 0xB1, 0xCC, 0xFD, 0x33, 0xB1, 0xDF,
                                                             0xF8, 0x0F, 0x92, 0xA3, 0x09, 0xD7, 0xBE, 0xC1, 0xC1, 0x12, 0xED, 0xD8, 0xF4, 0x45, 0x75, 0x08,
                                                             0x03, 0x32, 0xF9, 0x4C, 0x95, 0x87, 0xB8, 0x7A, 0xF8, 0x58, 0x8C, 0xA0, 0xA8, 0x7F, 0x54, 0x9B,
                                                             0xF2, 0xC7, 0x77, 0xA3, 0x50, 0xB0, 0x7E, 0x20, 0x06, 0xD3, 0xDA, 0xD9, 0x29, 0x2A, 0x0C, 0x1C };
static uint8_t test_encrypt_diff_size_small_buf[TEST_SMALL_PAYLOAD_SIZE] = { 0x75, 0x2E, 0x8D, 0xD0, 0x43, 0xE2, 0x5F, 0xBA, 0xF3, 0x92, 0xB1, 0xCC, 0xFD, 0x33, 0xB1, 0xDF };
static uint8_t test_encrypt_diff_size_large_buf[TEST_MAX_PAYLOAD_SIZE] = { 0x75, 0x2E, 0x8D, 0xD0, 0x43, 0xE2, 0x5F, 0xBA, 0xF3, 0x92, 0xB1, 0xCC, 0xFD, 0x33, 0xB1, 0xDF, 0xF8, 0x0F,
                                                                           0x92, 0xA3, 0x09, 0xD7, 0xBE, 0xC1, 0xC1, 0x12, 0xED, 0xD8, 0xF4, 0x45, 0x75, 0x08, 0x03, 0x32, 0xF9, 0x4C,
                                                                           0x95, 0x87, 0xB8, 0x7A, 0xF8, 0x58, 0x8C, 0xA0, 0xA8, 0x7F, 0x54, 0x9B, 0xF2, 0xC7, 0x77, 0xA3, 0x50, 0xB0,
                                                                           0x7E, 0x20, 0x06, 0xD3, 0xDA, 0xD9, 0x29, 0x2A, 0x0C, 0x1C };
static uint8_t test_decrypt_basic_buf[TEST_PAYLOAD_SIZE] = { 0x2A, 0x00, 0x14, 0xBF, 0x06, 0xEC, 0x04, 0x95, 0xAA, 0x57, 0xEA, 0xB8, 0x5A, 0xC3, 0xAA, 0xBE, 0xF0, 0x5F, 0xB1, 0x51, 0x62,
                                                             0x99, 0x75, 0x5A, 0xAD, 0x97, 0xE0, 0x5F, 0x7A, 0xE8, 0x5B, 0xBC, 0x7B, 0x1D, 0xD5, 0xA1, 0x09, 0xCF, 0x7C, 0x13, 0x58, 0x34,
                                                             0xDA, 0xD2, 0x08, 0x65, 0x03, 0x02, 0x3C, 0xBB, 0x87, 0xFE, 0xE6, 0x2D, 0x27, 0x43, 0xE2, 0x22, 0x19, 0x9C, 0x3C, 0xE7, 0xE2, 0x52 };

static uint8_t test_join_accept_basic_decrypt_buf[17] = { 0x20, 0xC1, 0x0D, 0x45, 0xA6, 0x90, 0x47, 0x2B, 0xEB, 0xEF, 0x28, 0x7A, 0xDD, 0xA8, 0x1E, 0x7B, 0x79 };
static uint8_t test_derive_skeys_nwk_encrypt_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0xEE, 0x1E, 0x78, 0x7F, 0x46, 0xBF, 0x54, 0xC8, 0xBF, 0x4C, 0xD0, 0xF7, 0xFC, 0xD3, 0xF3, 0x2C };
static uint8_t test_derive_skeys_app_encrypt_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0xE8, 0x3A, 0x38, 0xDB, 0x09, 0x52, 0xE5, 0x9B, 0x7A, 0x7B, 0x82, 0x27, 0xFA, 0x06, 0x07, 0xDB };
static uint8_t test_get_class_b_rand_buf[TEST_RAND_SIZE] = { 0x56, 0xA1, 0xFE, 0xF6, 0x0B, 0xB3, 0xD9, 0xE6, 0x2B, 0x30, 0x62, 0xD7, 0x70, 0x4F, 0x99, 0xEA };
static uint8_t test_service_encrypt_basic_buf[TEST_PAYLOAD_SIZE] = { 0xB0, 0x3C, 0x44, 0x3E, 0x29, 0x53, 0xE0, 0x10, 0x0B, 0xE1, 0x4F, 0x2D, 0x18, 0xB8, 0x78, 0x2A,
                                                                     0x6F, 0x04, 0x63, 0xA9, 0xC2, 0x98, 0x06, 0x6F, 0x0F, 0x46, 0xEF, 0x05, 0x5D, 0x2C, 0x03, 0x45,
                                                                     0x6F, 0x5D, 0xBE, 0x44, 0x28, 0xA9, 0xA7, 0xB2, 0xFE, 0x7D, 0x37, 0x6D, 0xB6, 0xB6, 0xE3, 0x61,
                                                                     0xC1, 0x14, 0xCD, 0xA5, 0xFC, 0xB8, 0x54, 0x25, 0x77, 0xFF, 0x52, 0x62, 0x36, 0xA4, 0x39, 0xC4 };
static uint8_t test_service_encrypt_diff_size_small_buf[TEST_SMALL_PAYLOAD_SIZE] = { 0xB0, 0x3C, 0x44, 0x3E, 0x29, 0x53, 0xE0, 0x10, 0x0B, 0xE1, 0x4F, 0x2D, 0x18, 0xB8, 0x78, 0x2A };
static uint8_t test_service_encrypt_diff_size_large_buf[TEST_MAX_PAYLOAD_SIZE] = { 0xB0, 0x3C, 0x44, 0x3E, 0x29, 0x53, 0xE0, 0x10, 0x0B, 0xE1, 0x4F, 0x2D, 0x18, 0xB8, 0x78, 0x2A,
                                                                                   0x6F, 0x04, 0x63, 0xA9, 0xC2, 0x98, 0x06, 0x6F, 0x0F, 0x46, 0xEF, 0x05, 0x5D, 0x2C, 0x03, 0x45,
                                                                                   0x6F, 0x5D, 0xBE, 0x44, 0x28, 0xA9, 0xA7, 0xB2, 0xFE, 0x7D, 0x37, 0x6D, 0xB6, 0xB6, 0xE3, 0x61,
                                                                                   0xC1, 0x14, 0xCD, 0xA5, 0xFC, 0xB8, 0x54, 0x25, 0x77, 0xFF, 0x52, 0x62, 0x36, 0xA4, 0x39, 0xC4 };
static uint8_t test_verify_mic_buf[TEST_MIC_SIZE] = { 0x11, 0x93, 0xBD, 0xB9 };
static uint8_t test_set_key_basic_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0x75, 0x2E, 0x8D, 0xD0, 0x43, 0xE2, 0x5F, 0xBA, 0xF3, 0x92, 0xB1, 0xCC, 0xFD, 0x33, 0xB1, 0xDF };
static uint8_t test_set_app_key_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0xE8, 0x3A, 0x38, 0xDB, 0x09, 0x52, 0xE5, 0x9B, 0x7A, 0x7B, 0x82, 0x27, 0xFA, 0x06, 0x07, 0xDB };
static uint8_t test_derive_multicast_app_key_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0x5F, 0xB9, 0xB6, 0x4C, 0x27, 0x07, 0xC9, 0x8A, 0x32, 0x63, 0xE5, 0xB6, 0x81, 0xFF, 0xC6, 0xF1 };
static uint8_t test_derive_multicast_nwk_key_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0xEA, 0xFF, 0x94, 0x43, 0xB4, 0xA3, 0xE4, 0xCA, 0xCA, 0x15, 0xAF, 0xCF, 0xBD, 0x4C, 0xCD, 0x91 };
static uint8_t test_derive_relay_key_buf[TEST_ENCRYPT_BUFF_SIZE] = { 0x2B, 0xE8, 0xDE, 0xEE, 0x38, 0x25, 0x10, 0xF4, 0xD1, 0x9C, 0x38, 0xE8, 0xE1, 0xDD, 0x2F, 0x7F };
#endif
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int smtc_modem_crypto_run_all_tests(void)
{
  UNITY_BEGIN();

  // Test payload encryption functions
  RUN_TEST(test_smtc_modem_crypto_payload_encrypt_basic);
  RUN_TEST(test_smtc_modem_crypto_payload_encrypt_decrypt_roundtrip);
  RUN_TEST(test_smtc_modem_crypto_payload_encrypt_different_sizes);

  // Test payload decryption functions
  RUN_TEST(test_smtc_modem_crypto_payload_decrypt_basic);

  // Test join MIC computation
  RUN_TEST(test_smtc_modem_crypto_compute_join_mic_basic);

  // Test join accept processing
  printf("Testing join accept processing...\n");
  RUN_TEST(test_smtc_modem_crypto_process_join_accept_basic);

  // Test session key derivation
  printf("Testing session key derivation...\n");
  RUN_TEST(test_smtc_modem_crypto_derive_skeys_basic);

  // Test MIC verification
  printf("Testing MIC verification...\n");
  RUN_TEST(test_smtc_modem_crypto_verify_mic_basic);
  RUN_TEST(test_smtc_modem_crypto_verify_mic_invalid_mic);
  RUN_TEST(test_smtc_modem_crypto_verify_mic_oversized_buffer);

  // Test MIC computation and addition
  printf("Testing MIC computation and addition...\n");
  RUN_TEST(test_smtc_modem_crypto_compute_and_add_mic_basic);

  // Test key management
  printf("Testing key management...\n");
  RUN_TEST(test_smtc_modem_crypto_set_key_basic);
  RUN_TEST(test_smtc_modem_crypto_set_key_app_key_derivation);

  // Test multicast key derivation
  printf("Testing multicast key derivation...\n");
  RUN_TEST(test_smtc_modem_crypto_derive_multicast_session_keys_basic);
  RUN_TEST(test_smtc_modem_crypto_derive_multicast_session_keys_invalid_params);

  // Test relay key derivation
  printf("Testing relay key derivation...\n");
  RUN_TEST(test_smtc_modem_crypto_derive_relay_session_keys_basic);

  // Test Class B rand generation
  printf("Testing Class B rand generation...\n");
  RUN_TEST(test_smtc_modem_crypto_get_class_b_rand_basic);

  // Test service encryption
  printf("Testing service encryption...\n");
  RUN_TEST(test_smtc_modem_crypto_service_encrypt_basic);
  RUN_TEST(test_smtc_modem_crypto_service_encrypt_different_sizes);

  printf("All SMTC Modem Crypto tests completed!\n");
  return UNITY_END();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test basic payload encryption functionality
 */
static void test_smtc_modem_crypto_payload_encrypt_basic(void)
{
  uint8_t payload[TEST_PAYLOAD_SIZE];
  uint8_t encrypted_payload[TEST_PAYLOAD_SIZE];
  smtc_modem_crypto_return_code_t result;

  // Setup test key
  setup_test_key(SMTC_SE_APP_S_KEY);

  // Create test payload
  create_test_payload(payload, TEST_PAYLOAD_SIZE);

  // Test encryption
  result = smtc_modem_crypto_payload_encrypt(
    payload,
    TEST_PAYLOAD_SIZE,
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    encrypted_payload,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Payload encryption should succeed");

  print_hex_buffer("Encrypted payload", encrypted_payload, TEST_PAYLOAD_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_encrypt_basic_buf, encrypted_payload, TEST_PAYLOAD_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Basic payload encryption test passed");
}

/**
 * @brief Test encrypt-decrypt roundtrip
 */
static void test_smtc_modem_crypto_payload_encrypt_decrypt_roundtrip(void)
{
  uint8_t original_payload[TEST_PAYLOAD_SIZE];
  uint8_t encrypted_payload[TEST_PAYLOAD_SIZE];
  uint8_t decrypted_payload[TEST_PAYLOAD_SIZE];
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_APP_S_KEY);
  create_test_payload(original_payload, TEST_PAYLOAD_SIZE);

  // Encrypt
  result = smtc_modem_crypto_payload_encrypt(
    original_payload,
    TEST_PAYLOAD_SIZE,
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    encrypted_payload,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Encryption should succeed");

  // Decrypt
  result = smtc_modem_crypto_payload_decrypt(
    encrypted_payload,
    TEST_PAYLOAD_SIZE,
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    decrypted_payload,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Decryption should succeed");

  // Verify roundtrip integrity
  TEST_ASSERT_EQUAL_MEMORY_MESSAGE(original_payload, decrypted_payload, TEST_PAYLOAD_SIZE,
                                   "Decrypted payload should match original");

  TEST_PASS_MESSAGE("Encrypt-decrypt roundtrip test passed");
}

/**
 * @brief Test payload encryption with different sizes
 */
static void test_smtc_modem_crypto_payload_encrypt_different_sizes(void)
{
  uint8_t small_payload[TEST_SMALL_PAYLOAD_SIZE];
  uint8_t encrypted_small[TEST_SMALL_PAYLOAD_SIZE];
  uint8_t large_payload[TEST_MAX_PAYLOAD_SIZE];
  uint8_t encrypted_large[TEST_MAX_PAYLOAD_SIZE];
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_APP_S_KEY);

  // Test small payload
  create_test_payload(small_payload, TEST_SMALL_PAYLOAD_SIZE);
  result = smtc_modem_crypto_payload_encrypt(
    small_payload,
    TEST_SMALL_PAYLOAD_SIZE,
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    encrypted_small,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Small payload encryption should succeed");

  print_hex_buffer("Small payload", encrypted_small, TEST_SMALL_PAYLOAD_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_encrypt_diff_size_small_buf, encrypted_small, TEST_SMALL_PAYLOAD_SIZE, "Small payload encryption failed - output doesn't match test vector");
#endif
  // Test large payload
  create_test_payload(large_payload, TEST_MAX_PAYLOAD_SIZE);
  result = smtc_modem_crypto_payload_encrypt(
    large_payload,
    TEST_MAX_PAYLOAD_SIZE,
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    encrypted_large,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Large payload encryption should succeed");
  print_hex_buffer("Large payload", encrypted_large, TEST_MAX_PAYLOAD_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_encrypt_diff_size_large_buf, encrypted_large, TEST_MAX_PAYLOAD_SIZE, "Large payload encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Different payload sizes encryption test passed");
}

/**
 * @brief Test basic payload decryption functionality
 */
static void test_smtc_modem_crypto_payload_decrypt_basic(void)
{
  uint8_t encrypted_payload[TEST_PAYLOAD_SIZE];
  uint8_t decrypted_payload[TEST_PAYLOAD_SIZE];
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_APP_S_KEY);
  create_test_payload(encrypted_payload, TEST_PAYLOAD_SIZE);

  result = smtc_modem_crypto_payload_decrypt(
    encrypted_payload,
    TEST_PAYLOAD_SIZE,
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_DOWNLINK,
    TEST_FRAME_COUNTER,
    decrypted_payload,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Payload decryption should succeed");
  print_hex_buffer("Encrypted payload", decrypted_payload, TEST_PAYLOAD_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_decrypt_basic_buf, decrypted_payload, TEST_PAYLOAD_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Basic payload decryption test passed");
}

/**
 * @brief Test basic join MIC computation
 */
static void test_smtc_modem_crypto_compute_join_mic_basic(void)
{
  uint8_t join_request[23] = {
    0x00, 0x04, 0x03, 0x02, 0x01, 0x04, 0x03, 0x02, 0x01,
    0x05, 0x04, 0x03, 0x02, 0x01, 0x05, 0x04, 0x03, 0x02,
    0x01, 0x02, 0x01, 0x01, 0x01
  };
  uint32_t computed_mic = 0;
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_NWK_KEY);

  result = smtc_modem_crypto_compute_join_mic(join_request, sizeof(join_request), &computed_mic, TEST_STACK_ID);

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Join MIC computation should succeed");
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, computed_mic, "Computed MIC should not be zero");

  char msg[64];
  snprintf(msg, sizeof(msg), "Computed join MIC: 0x%08lX", computed_mic);
  TEST_MESSAGE(msg);

  TEST_PASS_MESSAGE("Basic join MIC computation test passed");
}

/**
 * @brief Test basic join accept processing
 * Note: This test is simplified since join accept processing requires complex setup
 */
static void test_smtc_modem_crypto_process_join_accept_basic(void)
{
  // For now, just test that the function exists and handles parameters
  // Real join accept processing requires proper LoRaWAN context setup
  uint8_t encrypted_join_accept[17] = {
    0x20, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
  };
  uint8_t decrypted_join_accept[17] = { 0 };
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_NWK_KEY);

  result = smtc_modem_crypto_process_join_accept(
    encrypted_join_accept,
    sizeof(encrypted_join_accept),
    decrypted_join_accept,
    TEST_STACK_ID
    );

  // Accept any valid return code since test data is synthetic
  TEST_ASSERT_TRUE_MESSAGE((result == SMTC_MODEM_CRYPTO_RC_SUCCESS)
                           || (result == SMTC_MODEM_CRYPTO_RC_ERROR_SECURE_ELEMENT)
                           || (result == SMTC_MODEM_CRYPTO_RC_FAIL_MIC)
                           || (result == SMTC_MODEM_CRYPTO_RC_ERROR_BUF_SIZE),
                           "Join accept processing should return valid error code");

  printf("Join accept processing returned: %d (expected for test data)\n", result);

  print_hex_buffer("Decrypted content", decrypted_join_accept, 17);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_join_accept_basic_decrypt_buf, decrypted_join_accept, 17, "test_smtc_modem_crypto_process_join_accept_basic failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Basic join accept processing test passed");
}

/**
 * @brief Test basic session key derivation
 */
static void test_smtc_modem_crypto_derive_skeys_basic(void)
{
  smtc_modem_crypto_return_code_t result;
  uint8_t test_payload[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted_nwk[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted_app[TEST_ENCRYPT_BUFF_SIZE];

  setup_test_key(SMTC_SE_NWK_KEY);

  result = smtc_modem_crypto_derive_skeys(test_join_nonce, test_net_id, test_dev_nonce, TEST_STACK_ID);

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Session key derivation should succeed");

  create_test_payload(test_payload, sizeof(test_payload));

  // Test NWK_S_ENC_KEY
  result = smtc_modem_crypto_payload_encrypt(
    test_payload,
    sizeof(test_payload),
    SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    encrypted_nwk,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Derived NWK_S_ENC_KEY should work for encryption");
  print_hex_buffer("NWK_S_ENC_KEY encrypted", encrypted_nwk, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_derive_skeys_nwk_encrypt_buf, encrypted_nwk, TEST_ENCRYPT_BUFF_SIZE, "encryption failed - output doesn't match test vector");
#endif
  // Test APP_S_KEY
  result = smtc_modem_crypto_payload_encrypt(
    test_payload,
    sizeof(test_payload),
    SMTC_SE_APP_S_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    encrypted_app,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Derived APP_S_KEY should work for encryption");
  print_hex_buffer("APP_S_KEY encrypted", encrypted_app, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_derive_skeys_app_encrypt_buf, encrypted_app, TEST_ENCRYPT_BUFF_SIZE, "encryption failed - output doesn't match test vector");
#endif

  TEST_PASS_MESSAGE("Basic session key derivation test passed");
}

/**
 * @brief Test basic MIC verification
 */
static void test_smtc_modem_crypto_verify_mic_basic(void)
{
  uint8_t test_buffer[TEST_PAYLOAD_SIZE + TEST_MIC_SIZE];
  uint32_t computed_mic;
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_NWK_S_ENC_KEY);
  create_test_payload(test_buffer, TEST_PAYLOAD_SIZE);

  result = smtc_modem_crypto_compute_and_add_mic(
    test_buffer, TEST_PAYLOAD_SIZE, SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS, TEST_DIR_UPLINK, TEST_FRAME_COUNTER,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL(SMTC_MODEM_CRYPTO_RC_SUCCESS, result);

  computed_mic = (test_buffer[TEST_PAYLOAD_SIZE + 3] << 24)
                 | (test_buffer[TEST_PAYLOAD_SIZE + 2] << 16)
                 | (test_buffer[TEST_PAYLOAD_SIZE + 1] << 8)
                 | test_buffer[TEST_PAYLOAD_SIZE];

  printf("Computed MIC: 0x%08lX\n", computed_mic);
  print_hex_buffer("MIC bytes", test_buffer + TEST_PAYLOAD_SIZE, TEST_MIC_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_verify_mic_buf, test_buffer + TEST_PAYLOAD_SIZE, TEST_MIC_SIZE, "encryption failed - output doesn't match test vector");
#endif
  result = smtc_modem_crypto_verify_mic(
    test_buffer, TEST_PAYLOAD_SIZE, SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS, TEST_DIR_UPLINK, TEST_FRAME_COUNTER,
    computed_mic, TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "MIC verification should succeed with computed MIC");

  TEST_PASS_MESSAGE("MIC computation and verification cross-check passed");
}

/**
 * @brief Test MIC verification with invalid MIC
 */
static void test_smtc_modem_crypto_verify_mic_invalid_mic(void)
{
  uint8_t test_buffer[TEST_PAYLOAD_SIZE];
  uint32_t invalid_mic = 0xDEADBEEF;
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_NWK_S_ENC_KEY);
  create_test_payload(test_buffer, TEST_PAYLOAD_SIZE);

  result = smtc_modem_crypto_verify_mic(
    test_buffer,
    TEST_PAYLOAD_SIZE,
    SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    invalid_mic,
    TEST_STACK_ID
    );

  // Should fail with MIC mismatch
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_FAIL_MIC, result,
                            "Should fail with MIC mismatch for invalid MIC");

  TEST_PASS_MESSAGE("Invalid MIC verification test passed");
}

/**
 * @brief Test MIC verification with oversized buffer
 */
static void test_smtc_modem_crypto_verify_mic_oversized_buffer(void)
{
  uint8_t test_buffer[300];   // Larger than CRYPTO_MAXMESSAGE_SIZE (256)
  uint32_t test_mic = 0x12345678;
  smtc_modem_crypto_return_code_t result;

  create_test_payload(test_buffer, sizeof(test_buffer));

  result = smtc_modem_crypto_verify_mic(
    test_buffer,
    sizeof(test_buffer),
    SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    test_mic,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_ERROR_BUF_SIZE, result,
                            "Should return buffer size error for oversized buffer");

  TEST_PASS_MESSAGE("MIC verification oversized buffer test passed");
}

/**
 * @brief Test basic MIC computation and addition
 */
static void test_smtc_modem_crypto_compute_and_add_mic_basic(void)
{
  uint8_t test_buffer[TEST_PAYLOAD_SIZE + TEST_MIC_SIZE];
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_NWK_S_ENC_KEY);
  create_test_payload(test_buffer, TEST_PAYLOAD_SIZE);

  result = smtc_modem_crypto_compute_and_add_mic(
    test_buffer,
    TEST_PAYLOAD_SIZE,
    SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS,
    TEST_DIR_UPLINK,
    TEST_FRAME_COUNTER,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "MIC computation and addition should succeed");

  // Check that MIC was added (buffer should have non-zero values in MIC position)
  bool mic_added = false;
  for (int i = 0; i < TEST_MIC_SIZE; i++) {
    if (test_buffer[TEST_PAYLOAD_SIZE + i] != 0) {
      mic_added = true;
      break;
    }
  }
  TEST_ASSERT_TRUE_MESSAGE(mic_added, "MIC should be added to buffer");

  print_hex_buffer("Buffer with MIC", test_buffer + TEST_PAYLOAD_SIZE, TEST_MIC_SIZE);

  TEST_PASS_MESSAGE("Basic MIC computation and addition test passed");
}

/**
 * @brief Test basic key setting
 */
static void test_smtc_modem_crypto_set_key_basic(void)
{
  smtc_modem_crypto_return_code_t result;

  result = smtc_modem_crypto_set_key(SMTC_SE_NWK_S_ENC_KEY, test_key, TEST_STACK_ID);
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Key setting should succeed");

  // VERIFY: Key can be used for encryption
  uint8_t test_data[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted[TEST_ENCRYPT_BUFF_SIZE];
  create_test_payload(test_data, sizeof(test_data));
  result = smtc_modem_crypto_payload_encrypt(
    test_data, 16, SMTC_SE_NWK_S_ENC_KEY,
    TEST_DEVICE_ADDRESS, TEST_DIR_UPLINK, TEST_FRAME_COUNTER,
    encrypted, TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Set key should be usable for encryption");

  print_hex_buffer("Set key encrypted output", encrypted, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_set_key_basic_buf, encrypted, TEST_ENCRYPT_BUFF_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Key setting and verification test passed");
}
/**
 * @brief Test APP_KEY setting triggers multicast key derivation
 */
static void test_smtc_modem_crypto_set_key_app_key_derivation(void)
{
  smtc_modem_crypto_return_code_t result;

  // Set APP_KEY (root key)
  result = smtc_modem_crypto_set_key(SMTC_SE_APP_KEY, test_key, TEST_STACK_ID);
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "APP_KEY setting should succeed");

  // VERIFY: APP_S_KEY (derived from APP_KEY) can be used for encryption
  uint8_t test_data[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted_app[TEST_ENCRYPT_BUFF_SIZE];
  create_test_payload(test_data, sizeof(test_data));

  result = smtc_modem_crypto_payload_encrypt(
    test_data, sizeof(test_data), SMTC_SE_APP_S_KEY,  // Use derived session key
    TEST_DEVICE_ADDRESS, TEST_DIR_UPLINK, TEST_FRAME_COUNTER,
    encrypted_app, TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Derived APP_S_KEY should work for encryption");

  print_hex_buffer("APP_S_KEY encrypted", encrypted_app, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_set_app_key_buf, encrypted_app, TEST_ENCRYPT_BUFF_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("APP_KEY setting with APP_S_KEY verification test passed");
}

/**
 * @brief Test basic multicast session key derivation
 */
static void test_smtc_modem_crypto_derive_multicast_session_keys_basic(void)
{
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_MC_KEY_0);

  result = smtc_modem_crypto_derive_multicast_session_keys(
    SMTC_MODEM_CRYPTO_MULTICAST_0_ADDR,
    TEST_MULTICAST_ADDRESS,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Multicast session key derivation should succeed");

  // VERIFY: Derived multicast keys work for encryption
  uint8_t test_payload[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted_mc_app[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted_mc_nwk[TEST_ENCRYPT_BUFF_SIZE];
  create_test_payload(test_payload, sizeof(test_payload));

  // Test MC_APP_S_KEY_0
  result = smtc_modem_crypto_payload_encrypt(
    test_payload, sizeof(test_payload), SMTC_SE_MC_APP_S_KEY_0,
    TEST_MULTICAST_ADDRESS, TEST_DIR_DOWNLINK, 100,
    encrypted_mc_app, TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Multicast session key derivation should succeed");
  // Test MC_NWK_S_KEY_0
  result = smtc_modem_crypto_payload_encrypt(
    test_payload, sizeof(test_payload), SMTC_SE_MC_NWK_S_KEY_0,
    TEST_MULTICAST_ADDRESS, TEST_DIR_DOWNLINK, 100,
    encrypted_mc_nwk, TEST_STACK_ID
    );
  print_hex_buffer("APP_S_KEY encrypted", encrypted_mc_app, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_derive_multicast_app_key_buf, encrypted_mc_app, TEST_ENCRYPT_BUFF_SIZE, "encryption app failed - output doesn't match test vector");
#endif
  print_hex_buffer("NWK_S_KEY encrypted", encrypted_mc_nwk, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_derive_multicast_nwk_key_buf, encrypted_mc_nwk, TEST_ENCRYPT_BUFF_SIZE, "encryption nwk failed - output doesn't match test vector");
#endif
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Multicast session key derivation should succeed");

  TEST_PASS_MESSAGE("Multicast session key derivation verified");
}

/**
 * @brief Test multicast session key derivation with invalid parameters
 */
static void test_smtc_modem_crypto_derive_multicast_session_keys_invalid_params(void)
{
  smtc_modem_crypto_return_code_t result;

  // Test with zero multicast address
  result = smtc_modem_crypto_derive_multicast_session_keys(
    SMTC_MODEM_CRYPTO_MULTICAST_0_ADDR,
    0,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_ERROR_NPE, result,
                            "Should fail with zero multicast address");

  // Test with invalid address ID
  result = smtc_modem_crypto_derive_multicast_session_keys(
    (smtc_modem_crypto_addr_id_t)99,
    TEST_MULTICAST_ADDRESS,
    TEST_STACK_ID
    );
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_ERROR_INVALID_ADDR_ID, result,
                            "Should fail with invalid address ID");

  TEST_PASS_MESSAGE("Multicast session key derivation invalid parameters test passed");
}

/**
 * @brief Test basic relay session key derivation
 */
static void test_smtc_modem_crypto_derive_relay_session_keys_basic(void)
{
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_NWK_S_ENC_KEY);

  result = smtc_modem_crypto_derive_relay_session_keys(TEST_DEVICE_ADDRESS, TEST_STACK_ID);
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Relay session key derivation should succeed");

  uint8_t test_data[TEST_ENCRYPT_BUFF_SIZE];
  uint8_t encrypted_relay[TEST_ENCRYPT_BUFF_SIZE];
  create_test_payload(test_data, sizeof(test_data));

  result = smtc_modem_crypto_payload_encrypt(
    test_data, sizeof(test_data), SMTC_SE_RELAY_ROOT_WOR_S_KEY,
    TEST_DEVICE_ADDRESS, TEST_DIR_UPLINK, TEST_FRAME_COUNTER,
    encrypted_relay, TEST_STACK_ID
    );

  print_hex_buffer("encrypted_relay", encrypted_relay, TEST_ENCRYPT_BUFF_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_derive_relay_key_buf, encrypted_relay, TEST_ENCRYPT_BUFF_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Relay session key derivation verified");
}
/**
 * @brief Test basic Class B rand generation
 */
static void test_smtc_modem_crypto_get_class_b_rand_basic(void)
{
  uint8_t rand_output[TEST_RAND_SIZE] = { 0 };
  uint32_t beacon_time = 0x12345678;
  smtc_modem_crypto_return_code_t result;

  result = smtc_modem_crypto_get_class_b_rand(
    beacon_time,
    TEST_DEVICE_ADDRESS,
    rand_output,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Class B rand generation should succeed");

  // Check that rand was generated (not all zeros)
  bool rand_generated = false;
  for (int i = 0; i < TEST_RAND_SIZE; i++) {
    if (rand_output[i] != 0) {
      rand_generated = true;
      break;
    }
  }
  TEST_ASSERT_TRUE_MESSAGE(rand_generated, "Random values should be generated");

  print_hex_buffer("Class B rand", rand_output, TEST_RAND_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_get_class_b_rand_buf, rand_output, TEST_RAND_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Basic Class B rand generation test passed");
}

/**
 * @brief Test basic service encryption
 */
static void test_smtc_modem_crypto_service_encrypt_basic(void)
{
  uint8_t clear_buffer[TEST_PAYLOAD_SIZE];
  uint8_t encrypted_buffer[TEST_PAYLOAD_SIZE];
  uint8_t nonce[TEST_NONCE_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E
  };
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_APP_S_KEY);
  create_test_payload(clear_buffer, TEST_PAYLOAD_SIZE);

  result = smtc_modem_crypto_service_encrypt(
    clear_buffer,
    TEST_PAYLOAD_SIZE,
    nonce,
    encrypted_buffer,
    TEST_STACK_ID
    );

  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Service encryption should succeed");

  print_hex_buffer("Encrypted buffer", encrypted_buffer, TEST_PAYLOAD_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_service_encrypt_basic_buf, encrypted_buffer, TEST_PAYLOAD_SIZE, "test_smtc_modem_crypto_service_encrypt_basic failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Basic service encryption test passed");
}

/**
 * @brief Test service encryption with different sizes
 */
static void test_smtc_modem_crypto_service_encrypt_different_sizes(void)
{
  uint8_t small_clear[TEST_SMALL_PAYLOAD_SIZE];
  uint8_t small_encrypted[TEST_SMALL_PAYLOAD_SIZE];
  uint8_t large_clear[TEST_MAX_PAYLOAD_SIZE];
  uint8_t large_encrypted[TEST_MAX_PAYLOAD_SIZE];
  uint8_t nonce[TEST_NONCE_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E
  };
  smtc_modem_crypto_return_code_t result;

  setup_test_key(SMTC_SE_APP_S_KEY);

  // Test small buffer
  create_test_payload(small_clear, TEST_SMALL_PAYLOAD_SIZE);
  result = smtc_modem_crypto_service_encrypt(small_clear, TEST_SMALL_PAYLOAD_SIZE, nonce, small_encrypted, TEST_STACK_ID);
  print_hex_buffer("Encrypted buffer", small_encrypted, TEST_SMALL_PAYLOAD_SIZE);
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Small buffer service encryption should succeed");
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_service_encrypt_diff_size_small_buf, small_encrypted, TEST_SMALL_PAYLOAD_SIZE, "encryption failed - output doesn't match test vector");
#endif
  // Test large buffer
  create_test_payload(large_clear, TEST_MAX_PAYLOAD_SIZE);
  result = smtc_modem_crypto_service_encrypt(large_clear, TEST_MAX_PAYLOAD_SIZE, nonce, large_encrypted, TEST_STACK_ID);
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result,
                            "Large buffer service encryption should succeed");
  print_hex_buffer("Encrypted buffer", large_encrypted, TEST_MAX_PAYLOAD_SIZE);
#ifdef LBM_USE_HW_CRYPTO
  TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(test_service_encrypt_diff_size_large_buf, large_encrypted, TEST_MAX_PAYLOAD_SIZE, "encryption failed - output doesn't match test vector");
#endif
  TEST_PASS_MESSAGE("Service encryption different sizes test passed");
}

// -----------------------------------------------------------------------------
//                          Helper Function Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Setup a test key in the secure element
 */
static void setup_test_key(smtc_se_key_identifier_t key_id)
{
  smtc_modem_crypto_return_code_t result = smtc_modem_crypto_set_key(key_id, test_key, TEST_STACK_ID);
  TEST_ASSERT_EQUAL_MESSAGE(SMTC_MODEM_CRYPTO_RC_SUCCESS, result, "Test key setup should succeed");
}

/**
 * @brief Create a test payload with incrementing values
 */
static void create_test_payload(uint8_t* payload, uint16_t size)
{
  for (uint16_t i = 0; i < size; i++) {
    payload[i] = (uint8_t)(i & 0xFF);
  }
}

/**
 * @brief Print buffer contents in hex format for debugging
 */
static void print_hex_buffer(const char* prefix, const uint8_t* buffer, uint16_t size)
{
  char msg[256];
  int offset = snprintf(msg, sizeof(msg), "%s: ", prefix);

  for (uint16_t i = 0; i < size && offset < (sizeof(msg) - 3); i++) {
    offset += snprintf(msg + offset, sizeof(msg) - offset, "%02X ", buffer[i]);
  }

  TEST_MESSAGE(msg);
}
