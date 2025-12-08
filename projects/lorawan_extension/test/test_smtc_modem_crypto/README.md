# SMTC Modem Crypto Unit Tests

This directory contains comprehensive unit tests for the SMTC Modem Crypto module (`smtc_modem_crypto.c`), designed for EFR32xG28 and SX126X platforms using the Unity test framework.

## Overview

The test suite validates all major cryptographic functions in the SMTC Modem Crypto module including:

- **Payload Encryption/Decryption**: AES encryption and decryption of LoRaWAN payloads
- **MIC Computation/Verification**: Message Integrity Code generation and validation
- **Join Processing**: Join request MIC computation and join accept processing
- **Key Management**: Key setting and derivation for session keys
- **Multicast Support**: Multicast session key derivation
- **Class B Support**: Random number generation for Class B ping slots
- **Service Encryption**: Encryption for modem services (LFU and Stream)

## Files Structure

```
test_smtc_modem_crypto/
├── test_smtc_modem_crypto.h          # Main test header
├── test_smtc_modem_crypto.c          # Test implementations
├── smtc_crypto_test_runner.h         # Standalone test runner header
├── smtc_crypto_test_runner.c         # Standalone test runner implementation
├── app.h                             # Application header for standalone example
├── app.c                             # Application implementation
├── main.c                            # Main entry point for standalone example
├── smtc_modem_crypto.slcp            # Simplicity Studio project file
├── BUILD_CONFIG.md                   # Build configuration guide
└── README.md                         # This documentation file
```

## Test Categories

### 1. Payload Encryption Tests
- `test_smtc_modem_crypto_payload_encrypt_basic()` - Basic encryption functionality
- `test_smtc_modem_crypto_payload_encrypt_null_pointers()` - Null pointer handling
- `test_smtc_modem_crypto_payload_encrypt_decrypt_roundtrip()` - Encrypt-decrypt integrity
- `test_smtc_modem_crypto_payload_encrypt_different_sizes()` - Various payload sizes

### 2. Payload Decryption Tests
- `test_smtc_modem_crypto_payload_decrypt_basic()` - Basic decryption functionality
- `test_smtc_modem_crypto_payload_decrypt_null_pointers()` - Error handling

### 3. Join Processing Tests
- `test_smtc_modem_crypto_compute_join_mic_basic()` - Join MIC computation
- `test_smtc_modem_crypto_compute_join_mic_null_pointers()` - Error handling
- `test_smtc_modem_crypto_process_join_accept_basic()` - Join accept processing
- `test_smtc_modem_crypto_process_join_accept_null_pointers()` - Error handling

### 4. Session Key Derivation Tests
- `test_smtc_modem_crypto_derive_skeys_basic()` - Basic key derivation
- `test_smtc_modem_crypto_derive_skeys_null_pointers()` - Error handling

### 5. MIC Verification Tests
- `test_smtc_modem_crypto_verify_mic_basic()` - Basic MIC verification
- `test_smtc_modem_crypto_verify_mic_invalid_mic()` - Invalid MIC handling
- `test_smtc_modem_crypto_verify_mic_null_pointers()` - Null pointer handling
- `test_smtc_modem_crypto_verify_mic_oversized_buffer()` - Buffer size limits

### 6. MIC Computation Tests
- `test_smtc_modem_crypto_compute_and_add_mic_basic()` - MIC computation and addition
- `test_smtc_modem_crypto_compute_and_add_mic_null_pointers()` - Error handling

### 7. Key Management Tests
- `test_smtc_modem_crypto_set_key_basic()` - Basic key setting
- `test_smtc_modem_crypto_set_key_null_pointer()` - Null pointer handling
- `test_smtc_modem_crypto_set_key_app_key_derivation()` - APP_KEY multicast derivation

### 8. Multicast Key Tests
- `test_smtc_modem_crypto_derive_multicast_session_keys_basic()` - Multicast key derivation
- `test_smtc_modem_crypto_derive_multicast_session_keys_invalid_params()` - Parameter validation

### 9. Relay Key Tests
- `test_smtc_modem_crypto_derive_relay_session_keys_basic()` - Relay key derivation

### 10. Class B Tests
- `test_smtc_modem_crypto_get_class_b_rand_basic()` - Class B random generation
- `test_smtc_modem_crypto_get_class_b_rand_null_pointer()` - Error handling

### 11. Service Encryption Tests
- `test_smtc_modem_crypto_service_encrypt_basic()` - Basic service encryption
- `test_smtc_modem_crypto_service_encrypt_null_pointers()` - Error handling
- `test_smtc_modem_crypto_service_encrypt_different_sizes()` - Various buffer sizes

## Soft Secure Element

The test suite uses the built-in soft secure element implementation (`soft_se.c`) that:

- Provides real AES encryption/decryption operations
- Implements CMAC computation and verification
- Manages secure key storage and derivation
- Supports all LoRaWAN cryptographic operations
- Uses software-based crypto algorithms (AES, CMAC)

### Soft Secure Element Features

- **AES-128 Encryption**: Full hardware-equivalent AES implementation
- **CMAC Operations**: Message authentication code generation and verification
- **Key Management**: Secure storage and derivation of LoRaWAN keys
- **Multi-stack Support**: Supports multiple LoRaWAN stack instances
- **Context Persistence**: Save/restore secure element state

## Usage

### Option 1: Standalone Test Runner (Recommended)

Use the included standalone test runner for independent execution:

```c
#include "smtc_crypto_test_runner.h"

int main(void) {
    // Initialize system
    sl_system_init();

    // Initialize and run crypto tests
    smtc_crypto_test_runner_init();

    // Main loop
    while (1) {
        smtc_crypto_test_runner_process_action();
        // ... other system tasks ...
    }
}
```

### Option 2: Integration with Test Runner

Add the crypto tests to your main test runner:

```c
#include "test_smtc_modem_crypto/test_smtc_modem_crypto.h"

void run_all_tests(void) {
    // ... other tests ...

    // Run crypto tests
    int crypto_failures = smtc_modem_crypto_run_all_tests();

    // ... handle results ...
}
```

### Build Configuration

When building for unit tests, ensure:

1. Include the test directory in your build path
2. Link against Unity test framework
3. Include the soft secure element implementation from `components/SWL2001/lbm_lib/smtc_modem_core/smtc_modem_crypto/soft_secure_element/`

### Example Build Flags

```makefile
# For unit test builds
CFLAGS += -DUNITY_INCLUDE_CONFIG_H
INCLUDES += -Itest/test_smtc_modem_crypto
INCLUDES += -Icomponents/SWL2001/lbm_lib/smtc_modem_core/smtc_modem_crypto/soft_secure_element
```

## Test Data and Vectors

The tests use predefined test vectors and data patterns:

- **Test Key**: `{ 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }`
- **Test Device Address**: `0xABCDEF01`
- **Test Frame Counter**: `0x12345678`
- **Test Payloads**: Incrementing byte patterns of various sizes

## Expected Output

When all tests pass, you'll see output similar to:

```
=== Running SMTC Modem Crypto Unit Tests ===

test_smtc_modem_crypto_payload_encrypt_basic:PASS
test_smtc_modem_crypto_payload_encrypt_null_pointers:PASS
test_smtc_modem_crypto_payload_encrypt_decrypt_roundtrip:PASS
...
test_smtc_modem_crypto_service_encrypt_different_sizes:PASS

=== Test Summary ===
32 Tests 0 Failures 0 Ignored

All SMTC Modem Crypto tests PASSED!
```

## Customization and Extension

### Adding New Tests

1. Add test function declaration in `test_smtc_modem_crypto.h`
2. Implement test function in `test_smtc_modem_crypto.c`
3. Add `RUN_TEST()` call in `smtc_modem_crypto_run_all_tests()`

### Mock Behavior Control

Control mock behavior for negative testing:

```c
// Force encryption failures
mock_smtc_secure_element_set_behavior(MOCK_SE_FORCE_FAIL_ENCRYPT);

// Force CMAC failures
mock_smtc_secure_element_set_behavior(MOCK_SE_FORCE_FAIL_CMAC);

// Reset to normal behavior
mock_smtc_secure_element_set_behavior(0);
```

### Verification Examples

```c
// Verify function was called
uint32_t call_count = mock_smtc_secure_element_get_call_count("smtc_secure_element_aes_encrypt");
TEST_ASSERT_GREATER_THAN(0, call_count);

// Reset call counters
mock_smtc_secure_element_clear_counters();
```

## Platform Specific Notes

### EFR32xG28
- Tests are designed to work with EFR32xG28 secure element capabilities
- Mock provides simulation of hardware crypto operations
- Real hardware testing should use actual secure element when available

### SX126X
- Tests validate crypto operations used with SX126X radio
- Focus on LoRaWAN-specific cryptographic requirements
- Payload encryption/decryption for radio transmission

## Performance Considerations

- Tests use realistic payload sizes (16-255 bytes)
- Mock operations are fast for unit testing
- Real secure element operations may have different timing characteristics
- Consider timeout adjustments for hardware testing

## Security Notes

- Tests use well-known test vectors for reproducibility
- Soft secure element provides real cryptographic operations
- Implementation is suitable for development and testing
- Test keys should never be used in production
- For production deployment, consider using hardware secure element when available

## Troubleshooting

### Common Issues

1. **Soft secure element not found**: Ensure soft_secure_element directory is included in build paths
2. **Include path errors**: Add test directory and soft secure element paths to includes
3. **Unity framework missing**: Ensure Unity is properly linked
4. **Memory issues**: Check buffer sizes in test data

### Debug Output

Tests include hex buffer printing for debugging:

```c
print_hex_buffer("Original payload", payload, 16);
print_hex_buffer("Encrypted payload", encrypted_payload, 16);
```

Enable Unity verbose output for detailed test information.

## Integration Testing

These tests use real cryptographic operations and can be extended for integration testing:

1. Add hardware secure element support for full hardware testing
2. Add performance benchmarking for crypto operations
3. Test with real LoRaWAN test vectors from specification
4. Validate against LoRaWAN specification compliance requirements

## Compliance

Tests help validate compliance with:
- LoRaWAN 1.0.x specification cryptographic requirements
- AES-128 encryption standards
- CMAC message authentication
- Key derivation procedures as per LoRaWAN specification
