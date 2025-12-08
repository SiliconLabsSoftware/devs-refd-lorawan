/**
 * @file      smtc_secure_element_efr32.c
 *
 * @brief     EFR32 Secure Element implementation using Silicon Labs hardware crypto
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 * Modified for EFR32XG28 hardware crypto support
 * Copyright 2025 Silicon Laboratories Inc. www.silabs.com
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

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type
#include <stdlib.h>   // for malloc/free

#include "smtc_secure_element.h"

// Silicon Labs Hardware Crypto includes
#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"
#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_types.h"
#include "sl_se_manager_defines.h"
#include "sl_memory_manager.h"

#include "smtc_modem_hal.h"
#include "smtc_hal_dbg_trace.h"

#include <string.h>  //for memset, memcpy

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */
#define WRAPPER_NAME(function) __wrap_##function

/*!
 * Number of keys supported in EFR32 secure element
 */
#define EFR32_SE_NUMBER_OF_KEYS 27

/*!
 * JoinAccept frame maximum size
 */
#define JOIN_ACCEPT_FRAME_MAX_SIZE 33

/*!
 * Lorawan MIC size
 */
#define LORWAN_MIC_FIELD_SIZE 4

/*!
 * Lorawan MHDR SIZE
 */
#define LORAMAC_MHDR_FIELD_SIZE 1

/*!
 * EFR32 SE key ID base - using application key range
 */
#define EFR32_SE_KEY_ID_BASE 0x81000000

/*!
 * AES block size in bytes
 */
#define AES_BLOCK_SIZE                       16

/*!
 * LoRaWAN B0 block size (for CMAC calculation)
 */
#define LORAWAN_B0_BLOCK_SIZE                16

/*!
 * AES block alignment mask (block_size - 1)
 */
#define AES_BLOCK_ALIGNMENT_MASK             15

/*!
 * LoRaWAN JoinAccept DLSettings byte offset (from start of message)
 */
#define LORAWAN_JOIN_ACCEPT_DLSETTINGS_OFFSET    11

/*!
 * LoRaWAN version minor bit mask in DLSettings byte
 * Bit 7: 0 = LoRaWAN 1.0.x, 1 = LoRaWAN 1.1.x
 */
#define LORAWAN_VERSION_MINOR_BIT_MASK           0x80

/*!
 * LoRaWAN 1.0.x version minor value
 */
#define LORAWAN_VERSION_MINOR_1_0                0

/*!
 * LoRaWAN 1.1.x version minor value
 */
#define LORAWAN_VERSION_MINOR_1_1                1

#define EFR32_SE_KEY_LIST                                                                  \
  {                                                                                        \
    { .key_id = SMTC_SE_APP_KEY, .is_set = false, .is_hardware_key = false },              \
    { .key_id = SMTC_SE_NWK_KEY, .is_set = false, .is_hardware_key = false },              \
    { .key_id = SMTC_SE_J_S_INT_KEY, .is_set = false, .is_hardware_key = false },          \
    { .key_id = SMTC_SE_J_S_ENC_KEY, .is_set = false, .is_hardware_key = false },          \
    { .key_id = SMTC_SE_F_NWK_S_INT_KEY, .is_set = false, .is_hardware_key = false },      \
    { .key_id = SMTC_SE_S_NWK_S_INT_KEY, .is_set = false, .is_hardware_key = false },      \
    { .key_id = SMTC_SE_NWK_S_ENC_KEY, .is_set = false, .is_hardware_key = false },        \
    { .key_id = SMTC_SE_APP_S_KEY, .is_set = false, .is_hardware_key = false },            \
    { .key_id = SMTC_SE_MC_ROOT_KEY, .is_set = false, .is_hardware_key = false },          \
    { .key_id = SMTC_SE_MC_KE_KEY, .is_set = false, .is_hardware_key = false },            \
    { .key_id = SMTC_SE_MC_KEY_0, .is_set = false, .is_hardware_key = false },             \
    { .key_id = SMTC_SE_MC_APP_S_KEY_0, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_NWK_S_KEY_0, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_KEY_1, .is_set = false, .is_hardware_key = false },             \
    { .key_id = SMTC_SE_MC_APP_S_KEY_1, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_NWK_S_KEY_1, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_KEY_2, .is_set = false, .is_hardware_key = false },             \
    { .key_id = SMTC_SE_MC_APP_S_KEY_2, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_NWK_S_KEY_2, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_KEY_3, .is_set = false, .is_hardware_key = false },             \
    { .key_id = SMTC_SE_MC_APP_S_KEY_3, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_MC_NWK_S_KEY_3, .is_set = false, .is_hardware_key = false },       \
    { .key_id = SMTC_SE_RELAY_ROOT_WOR_S_KEY, .is_set = false, .is_hardware_key = false }, \
    { .key_id = SMTC_SE_RELAY_WOR_S_INT_KEY, .is_set = false, .is_hardware_key = false },  \
    { .key_id = SMTC_SE_RELAY_WOR_S_ENC_KEY, .is_set = false, .is_hardware_key = false },  \
    { .key_id = SMTC_SE_DATA_BLOCK_INT_KEY, .is_set = false, .is_hardware_key = false },   \
    { .key_id = SMTC_SE_SLOT_RAND_ZERO_KEY, .is_set = false, .is_hardware_key = false },   \
  }

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/**
 * @brief EFR32 key storage element structure
 */
typedef struct efr32_se_key_s{
  smtc_se_key_identifier_t key_id;                        //!< Key identifier
  sl_se_key_descriptor_t   se_key_desc;                  //!< SE Manager key descriptor
  uint8_t                  key_buffer[SMTC_SE_KEY_SIZE];   //!< Key storage buffer
  bool                     is_set;                        //!< Flag indicating if key is set
  bool                     is_hardware_key;               //!< Flag indicating if key is stored in hardware
} efr32_se_key_t;

/**
 * @brief EFR32 Secure element context structure
 */
typedef struct efr32_se_context_s{
  efr32_se_key_t key_list[EFR32_SE_NUMBER_OF_KEYS];      //!< Key list (27 keys as per SMTC spec)
  uint8_t        dev_eui[SMTC_SE_EUI_SIZE];              //!< DevEUI storage
  uint8_t        join_eui[SMTC_SE_EUI_SIZE];             //!< JoinEUI storage
  uint8_t        pin[SMTC_SE_PIN_SIZE];                  //!< Pin storage
  sl_se_command_context_t cmd_ctx;                       //!< SE Manager command context
} efr32_se_context_t;

/**
 * @brief EFR32 secure element context
 */
static efr32_se_context_t efr32_se_ctx;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/**
 * @brief Get key from key list
 *
 * @param [in] key_id Key identifier
 * @param [in] key Pointer to the key
 * @return Secure element return code as defined in @ref smtc_se_return_code_t
 */
static smtc_se_return_code_t get_key_by_id(smtc_se_key_identifier_t key_id, efr32_se_key_t** key);

/**
 * @brief Set key in key list with hardware storage
 *
 * @param [in] key_id Key identifier
 * @param [in] key Key value
 * @return Secure element return code as defined in @ref smtc_se_return_code_t
 */
static smtc_se_return_code_t set_key_by_id(smtc_se_key_identifier_t key_id, const uint8_t key[SMTC_SE_KEY_SIZE]);

/**
 * @brief Initialize a hardware key in the SE Manager
 *
 * @param [in] key Pointer to the key structure
 * @param [in] key_data Key data to store
 * @return Secure element return code as defined in @ref smtc_se_return_code_t
 */
static smtc_se_return_code_t efr32_se_init_hardware_key(efr32_se_key_t* key, const uint8_t* key_data);

/**
 * @brief Compute CMAC using EFR32 hardware
 *
 * @param [in] key_desc SE key descriptor
 * @param [in] data Input data
 * @param [in] data_len Input data length
 * @param [out] mac Output MAC (16 bytes)
 * @return Secure element return code as defined in @ref smtc_se_return_code_t
 */
static smtc_se_return_code_t efr32_se_compute_cmac(const sl_se_key_descriptor_t* key_desc,
                                                   const uint8_t* data,
                                                   size_t data_len,
                                                   uint8_t* mac);

/**
 * @brief Perform AES encryption using EFR32 hardware
 *
 * @param [in] key_desc SE key descriptor
 * @param [in] input Input data (must be multiple of 16 bytes)
 * @param [in] input_len Input data length
 * @param [out] output Output buffer
 * @return Secure element return code as defined in @ref smtc_se_return_code_t
 */
static smtc_se_return_code_t efr32_se_aes_encrypt(const sl_se_key_descriptor_t* key_desc,
                                                  const uint8_t* input,
                                                  size_t input_len,
                                                  uint8_t* output);

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITIONS --------------------------------------------
 */
smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_init)(void)
{
  // Initialize SE Manager
  sl_status_t status = sl_se_init();
  if ( status != SL_STATUS_OK ) {
    printf("EFR32 SE Manager init failed: 0x%lx\n", status);
    return SMTC_SE_RC_ERROR;
  }

  // Initialize context
  memset(&efr32_se_ctx, 0, sizeof(efr32_se_ctx) );

  // Initialize key list
  efr32_se_key_t key_list_init[EFR32_SE_NUMBER_OF_KEYS] = EFR32_SE_KEY_LIST;
  memcpy(efr32_se_ctx.key_list, key_list_init, sizeof(key_list_init) );

  // Initialize SE command context
  status = sl_se_init_command_context(&efr32_se_ctx.cmd_ctx);
  if ( status != SL_STATUS_OK ) {
    printf("EFR32 SE command context init failed: 0x%lx\n", status);
    return SMTC_SE_RC_ERROR;
  }

  printf("EFR32 hardware crypto secure element initialized\n");
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_set_key)(smtc_se_key_identifier_t key_id, const uint8_t key[SMTC_SE_KEY_SIZE],
                                                                uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( key == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  return set_key_by_id(key_id, key);
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_compute_aes_cmac)(const uint8_t * mic_bx_buffer, const uint8_t * buffer,
                                                                         uint16_t size, smtc_se_key_identifier_t key_id,
                                                                         uint32_t * cmac, uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( (buffer == NULL) || (cmac == NULL) ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  efr32_se_key_t* key;
  smtc_se_return_code_t rc = get_key_by_id(key_id, &key);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  if ( !key->is_set ) {
    return SMTC_SE_RC_ERROR_INVALID_KEY_ID;
  }

  // Prepare data for CMAC calculation
  const uint8_t* cmac_data;
  size_t cmac_data_size = size;
  uint8_t* temp_data = NULL;

  if ( mic_bx_buffer != NULL ) {
    // Concatenate B0 block and data
    cmac_data_size = LORAWAN_B0_BLOCK_SIZE + size;
    temp_data = sl_malloc(cmac_data_size);
    if ( temp_data == NULL ) {
      return SMTC_SE_RC_ERROR;
    }
    memcpy(temp_data, mic_bx_buffer, LORAWAN_B0_BLOCK_SIZE);
    memcpy(temp_data + LORAWAN_B0_BLOCK_SIZE, buffer, size);
    cmac_data = temp_data;
  } else {
    cmac_data = buffer;
  }

  uint8_t mac[16];

  // Use EFR32 hardware CMAC
  smtc_se_return_code_t cmac_rc = efr32_se_compute_cmac(&key->se_key_desc, cmac_data, cmac_data_size, mac);

  if ( mic_bx_buffer != NULL ) {
    sl_free(cmac_data);
  }

  if ( cmac_rc != SMTC_SE_RC_SUCCESS ) {
    return cmac_rc;
  }

  // Copy first 4 bytes as CMAC result
  memcpy(cmac, mac, 4);

  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_verify_aes_cmac)(uint8_t * buffer, uint16_t size, uint32_t expected_cmac, // NOSONAR
                                                                        smtc_se_key_identifier_t key_id, uint8_t stack_id)
{
  if ( buffer == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  // Verify CMAC using hardware implementation
  uint32_t computed_cmac;
  smtc_se_return_code_t rc = smtc_secure_element_compute_aes_cmac(NULL, buffer, size, key_id, &computed_cmac, stack_id);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  if ( computed_cmac != expected_cmac ) {
    return SMTC_SE_RC_FAIL_CMAC;
  }

  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_aes_encrypt)(const uint8_t * buffer, uint16_t size,
                                                                    smtc_se_key_identifier_t key_id, uint8_t * enc_buffer,
                                                                    uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( (buffer == NULL) || (enc_buffer == NULL) ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  if ( size % 16 != 0 ) {
    return SMTC_SE_RC_ERROR_BUF_SIZE;
  }

  efr32_se_key_t* key;
  smtc_se_return_code_t rc = get_key_by_id(key_id, &key);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  if ( !key->is_set ) {
    return SMTC_SE_RC_ERROR_INVALID_KEY_ID;
  }

  // Use EFR32 hardware AES encryption
  smtc_se_return_code_t aes_rc = efr32_se_aes_encrypt(&key->se_key_desc, buffer, size, enc_buffer);

  if ( aes_rc != SMTC_SE_RC_SUCCESS ) {
    return aes_rc;
  }

  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_derive_and_store_key)(uint8_t * input, smtc_se_key_identifier_t rootkey_id, // NOSONAR
                                                                             smtc_se_key_identifier_t targetkey_id,
                                                                             uint8_t                  stack_id)
{
  if ( input == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  efr32_se_key_t* root_key;
  smtc_se_return_code_t rc = get_key_by_id(rootkey_id, &root_key);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  if ( !root_key->is_set ) {
    return SMTC_SE_RC_ERROR_INVALID_KEY_ID;
  }

  // Derive key by encrypting input with root key (AES-ECB)
  uint8_t derived_key[SMTC_SE_KEY_SIZE];
  rc = smtc_secure_element_aes_encrypt(input, SMTC_SE_KEY_SIZE, rootkey_id, derived_key, stack_id);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  // Store derived key
  return set_key_by_id(targetkey_id, derived_key);
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_derive_relay_session_keys)(uint32_t dev_addr, uint8_t stack_id)
{
  uint8_t block[16];
  memset(block, 0, sizeof(block) );
  block[0] = 0x01;

  smtc_se_return_code_t rc = smtc_secure_element_derive_and_store_key(block, SMTC_SE_NWK_S_ENC_KEY,
                                                                      SMTC_SE_RELAY_ROOT_WOR_S_KEY,
                                                                      stack_id);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }
  memset(block, 0, sizeof(block) );
  block[0] = 0x01;
  block[1] = ( uint8_t ) (dev_addr);
  block[2] = ( uint8_t ) (dev_addr >> 8);
  block[3] = ( uint8_t ) (dev_addr >> 16);
  block[4] = ( uint8_t ) (dev_addr >> 24);

  rc = smtc_secure_element_derive_and_store_key(block, SMTC_SE_RELAY_ROOT_WOR_S_KEY,
                                                SMTC_SE_RELAY_WOR_S_INT_KEY,
                                                stack_id);

  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  memset(block, 0, sizeof(block) );
  block[0] = 0x02;
  block[1] = ( uint8_t ) (dev_addr);
  block[2] = ( uint8_t ) (dev_addr >> 8);
  block[3] = ( uint8_t ) (dev_addr >> 16);
  block[4] = ( uint8_t ) (dev_addr >> 24);
  rc = smtc_secure_element_derive_and_store_key(block, SMTC_SE_RELAY_ROOT_WOR_S_KEY,
                                                SMTC_SE_RELAY_WOR_S_ENC_KEY,
                                                stack_id);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_process_join_accept)(smtc_se_join_req_identifier_t join_req_type,
                                                                            uint8_t joineui[SMTC_SE_EUI_SIZE], uint16_t dev_nonce, // NOSONAR
                                                                            const uint8_t * enc_join_accept,
                                                                            uint8_t enc_join_accept_size, uint8_t * dec_join_accept,
                                                                            uint8_t * version_minor, uint8_t stack_id)
{
  UNUSED(joineui);
  UNUSED(dev_nonce);
  if ((enc_join_accept == NULL) || (dec_join_accept == NULL) || (version_minor == NULL)) {
    return SMTC_SE_RC_ERROR_NPE;
  }
  if (enc_join_accept_size > JOIN_ACCEPT_FRAME_MAX_SIZE) {
    return SMTC_SE_RC_ERROR_BUF_SIZE;
  }

  // Determine decryption key
  smtc_se_key_identifier_t enckey_id = SMTC_SE_NWK_KEY;
  if (join_req_type != SMTC_SE_JOIN_REQ) {
    enckey_id = SMTC_SE_J_S_ENC_KEY;
  }

  memcpy(dec_join_accept, enc_join_accept, enc_join_accept_size);

  // Decrypt JoinAccept, skip MHDR
  if (smtc_secure_element_aes_encrypt(
        enc_join_accept + LORAMAC_MHDR_FIELD_SIZE,
        enc_join_accept_size - LORAMAC_MHDR_FIELD_SIZE,
        enckey_id,
        dec_join_accept + LORAMAC_MHDR_FIELD_SIZE,
        stack_id) != SMTC_SE_RC_SUCCESS) {
    return SMTC_SE_RC_FAIL_ENCRYPT;
  }

  *version_minor = ((dec_join_accept[LORAWAN_JOIN_ACCEPT_DLSETTINGS_OFFSET] & LORAWAN_VERSION_MINOR_BIT_MASK) == LORAWAN_VERSION_MINOR_BIT_MASK) ? LORAWAN_VERSION_MINOR_1_1 : LORAWAN_VERSION_MINOR_1_0;

  uint32_t mic = 0;
  mic = ((uint32_t)dec_join_accept[enc_join_accept_size - LORWAN_MIC_FIELD_SIZE] << 0);
  mic |= ((uint32_t)dec_join_accept[enc_join_accept_size - LORWAN_MIC_FIELD_SIZE + 1] << 8);
  mic |= ((uint32_t)dec_join_accept[enc_join_accept_size - LORWAN_MIC_FIELD_SIZE + 2] << 16);
  mic |= ((uint32_t)dec_join_accept[enc_join_accept_size - LORWAN_MIC_FIELD_SIZE + 3] << 24);

  // Verify mic
  if (*version_minor == 0) {
    // LoRaWAN 1.0.x: MIC = aes128_cmac(NwkKey, MHDR | ... | CFList)
    if (smtc_secure_element_verify_aes_cmac(
          dec_join_accept,
          (enc_join_accept_size - LORWAN_MIC_FIELD_SIZE),
          mic,
          SMTC_SE_NWK_KEY,
          stack_id) != SMTC_SE_RC_SUCCESS) {
      return SMTC_SE_RC_FAIL_CMAC;
    }
  } else {
    return SMTC_SE_RC_ERROR_INVALID_LORAWAM_SPEC_VERSION;
  }

  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_set_deveui)(const uint8_t deveui[SMTC_SE_EUI_SIZE], uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( deveui == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  memcpy(efr32_se_ctx.dev_eui, deveui, SMTC_SE_EUI_SIZE);
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_get_deveui)(uint8_t deveui[SMTC_SE_EUI_SIZE], uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( deveui == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  memcpy(deveui, efr32_se_ctx.dev_eui, SMTC_SE_EUI_SIZE);
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_set_joineui)(const uint8_t joineui[SMTC_SE_EUI_SIZE], uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( joineui == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  memcpy(efr32_se_ctx.join_eui, joineui, SMTC_SE_EUI_SIZE);
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_get_joineui)(uint8_t joineui[SMTC_SE_EUI_SIZE], uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( joineui == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  memcpy(joineui, efr32_se_ctx.join_eui, SMTC_SE_EUI_SIZE);
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_set_pin)(const uint8_t pin[SMTC_SE_PIN_SIZE], uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( pin == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  memcpy(efr32_se_ctx.pin, pin, SMTC_SE_PIN_SIZE);
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_get_pin)(uint8_t pin[SMTC_SE_PIN_SIZE], uint8_t stack_id)
{
  UNUSED(stack_id);

  if ( pin == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  memcpy(pin, efr32_se_ctx.pin, SMTC_SE_PIN_SIZE);
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_store_context)(uint8_t stack_id)
{
  UNUSED(stack_id);

  // Store context to NVM using HAL context functions
  smtc_modem_hal_context_store(CONTEXT_SECURE_ELEMENT, 0, (const uint8_t*)&efr32_se_ctx, sizeof(efr32_se_ctx) );
  return SMTC_SE_RC_SUCCESS;
}

smtc_se_return_code_t WRAPPER_NAME(smtc_secure_element_restore_context)(uint8_t stack_id)
{
  UNUSED(stack_id);

  // Restore context from NVM using HAL context functions
  smtc_modem_hal_context_restore(CONTEXT_SECURE_ELEMENT, 0, (uint8_t*)&efr32_se_ctx, sizeof(efr32_se_ctx) );

  // Re-initialize SE Manager after restore
  sl_status_t status = sl_se_init_command_context(&efr32_se_ctx.cmd_ctx);
  if ( status != SL_STATUS_OK ) {
    printf("EFR32 SE command context restore failed: 0x%lx\n", status);
    return SMTC_SE_RC_ERROR;
  }

  return SMTC_SE_RC_SUCCESS;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITIONS -------------------------------------------
 */
static smtc_se_return_code_t get_key_by_id(smtc_se_key_identifier_t key_id, efr32_se_key_t** key)
{
  for ( uint8_t i = 0; i < EFR32_SE_NUMBER_OF_KEYS; i++ ) {
    if ( efr32_se_ctx.key_list[i].key_id == key_id ) {
      *key = &(efr32_se_ctx.key_list[i]);
      return SMTC_SE_RC_SUCCESS;
    }
  }
  return SMTC_SE_RC_ERROR_INVALID_KEY_ID;
}

static smtc_se_return_code_t set_key_by_id(smtc_se_key_identifier_t key_id, const uint8_t key[SMTC_SE_KEY_SIZE])
{
  efr32_se_key_t* key_item;
  smtc_se_return_code_t rc = get_key_by_id(key_id, &key_item);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    return rc;
  }

  uint8_t final_key[SMTC_SE_KEY_SIZE];

  if ( (key_id == SMTC_SE_MC_KEY_0) || (key_id == SMTC_SE_MC_KEY_1)
       || (key_id == SMTC_SE_MC_KEY_2) || (key_id == SMTC_SE_MC_KEY_3) ) {
    rc = smtc_secure_element_aes_encrypt(
      key, SMTC_SE_KEY_SIZE, SMTC_SE_MC_KE_KEY, final_key, 0);
    if ( rc != SMTC_SE_RC_SUCCESS ) {
      return rc;
    }
  } else {
    memcpy(final_key, key, SMTC_SE_KEY_SIZE);
  }

  // Copy key value to buffer
  memcpy(key_item->key_buffer, final_key, SMTC_SE_KEY_SIZE);

  // Initialize hardware key
  rc = efr32_se_init_hardware_key(key_item, final_key);
  if ( rc != SMTC_SE_RC_SUCCESS ) {
    printf("Failed to initialize hardware key for key_id=%d\n", key_id);
    return rc;
  }

  key_item->is_set = true;
  return SMTC_SE_RC_SUCCESS;
}

static smtc_se_return_code_t efr32_se_init_hardware_key(efr32_se_key_t* key, const uint8_t* key_data)
{
  if ( key == NULL || key_data == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  // Initialize key descriptor for plaintext key storage
  sl_se_key_descriptor_t* desc = &key->se_key_desc;

  desc->type = SL_SE_KEY_TYPE_AES_128;
  desc->flags = 0;    // No special flags for symmetric AES key
  desc->storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
  desc->storage.location.buffer.pointer = key->key_buffer;
  desc->storage.location.buffer.size = SMTC_SE_KEY_SIZE;

  key->is_hardware_key = true;
  return SMTC_SE_RC_SUCCESS;
}

static smtc_se_return_code_t efr32_se_compute_cmac(const sl_se_key_descriptor_t* key_desc,
                                                   const uint8_t* data,
                                                   size_t data_len,
                                                   uint8_t* mac)
{
  if ( key_desc == NULL || data == NULL || mac == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  // Use SE Manager to compute CMAC
  sl_status_t status = sl_se_cmac(&efr32_se_ctx.cmd_ctx,
                                  key_desc,
                                  data,
                                  data_len,
                                  mac);

  if ( status != SL_STATUS_OK ) {
    return SMTC_SE_RC_ERROR;
  }

  return SMTC_SE_RC_SUCCESS;
}

static smtc_se_return_code_t efr32_se_aes_encrypt(const sl_se_key_descriptor_t* key_desc,
                                                  const uint8_t* input,
                                                  size_t input_len,
                                                  uint8_t* output)
{
  if ( key_desc == NULL || input == NULL || output == NULL ) {
    return SMTC_SE_RC_ERROR_NPE;
  }

  // Use SE Manager for AES-ECB encryption
  sl_status_t status = sl_se_aes_crypt_ecb(&efr32_se_ctx.cmd_ctx,
                                           key_desc,
                                           SL_SE_ENCRYPT,
                                           input_len,
                                           input,
                                           output);

  if ( status != SL_STATUS_OK ) {
    return SMTC_SE_RC_FAIL_ENCRYPT;
  }
  return SMTC_SE_RC_SUCCESS;
}

/* --- EOF ------------------------------------------------------------------ */
