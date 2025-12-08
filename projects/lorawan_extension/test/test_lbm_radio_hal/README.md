# Radio HAL Unit Tests

## Overview
This directory contains comprehensive unit tests for the Radio HAL implementation (`sx126x_hal.c`) using Unity testing framework. The tests focus on the 4 global API functions that interface with the SX126x radio chip through Silicon Labs wrapper functions.

## Directory Structure

```
test_lbm_radio_hal/
├── README.md                       # This guide
├── test_sl_lbm_radio_hal.c            # Unity framework test implementations
├── test_sl_lbm_radio_hal.h            # Test function declarations
├── sl_lbm_radio_hal_test_runner.c     # Unity test runner for app.c integration
└── sl_lbm_radio_hal_test_runner.h     # Test runner function declarations
```

## Tested API Functions

The unit tests focus on the 4 global API functions implemented in `sx126x_hal.c`:

1. **`sx126x_hal_write()`** - Write command and data to SX126x via SPI
2. **`sx126x_hal_read()`** - Read data from SX126x via SPI  
3. **`sx126x_hal_reset()`** - Reset the SX126x radio chip
4. **`sx126x_hal_wakeup()`** - Wake up the SX126x from sleep mode

#### Test Coverage:
- **sx126x_hal_write()** - 3 software tests + 2 hardware tests
  - Valid parameters
  - Zero command length
  - Sleep command (sets radio to sleep mode)
  - **[HW]** Real hardware communication test
  - **[HW]** Real sleep command execution

- **sx126x_hal_read()** - 2 software tests + 2 hardware tests
  - Valid parameters
  - Zero data length
  - **[HW]** Real hardware status read (expects mock 0x22 response)
  - **[HW]** Read radio version register (expects mock 0x22 response)

- **sx126x_hal_reset()** - 1 software tests + 1 hardware test
  - Valid context
  - **[HW]** Real hardware reset sequence (with reduced delays in test mode)

- **sx126x_hal_wakeup()** - 1 software tests + 1 hardware test
  - Valid context
  - **[HW]** Real hardware wakeup from sleep

**Total: 7 software tests + 6 hardware tests = 13 tests**

## Test Setup

### Step 1: Add LoRaWAN Extension to Simplicity SDK
### Step 2: Create Project "LoRaWAN Radio HAL Unit Tests"
### Step 3: Build and Flash to Device

## Expected Test Output (Unit Testing Mode)

```
Radio HAL Unity Test Runner
Testing Global API Functions in sl_radio_hal.c
============================================================

Running all test cases...
../sl_radio_hal_test_runner.c:52:test_sx126x_hal_write_ValidParams_ShouldReturnOK:PASS
../sl_radio_hal_test_runner.c:54:test_sx126x_hal_write_ZeroCommandLength_ShouldReturnOK:PASS
../sl_radio_hal_test_runner.c:55:test_sx126x_hal_write_SleepCommand_ShouldSetSleepMode:PASS
../sl_radio_hal_test_runner.c:59:test_sx126x_hal_read_ValidParams_ShouldReturnOK:PASS
../sl_radio_hal_test_runner.c:62:test_sx126x_hal_read_ZeroDataLength_ShouldReturnOK:PASS
../sl_radio_hal_test_runner.c:66:test_sx126x_hal_reset_ValidContext_ShouldReturnOK:PASS
../sl_radio_hal_test_runner.c:70:test_sx126x_hal_wakeup_ValidContext_ShouldReturnOK:PASS

[HW TEST] Testing sx126x_hal_write with real hardware...
[HW TEST] NOP command result: OK
../sl_radio_hal_test_runner.c:74:test_sx126x_hal_write_WithRealHardware_ShouldCommunicate:PASS

[HW TEST] Testing sx126x_hal_write with sleep command...
[HW TEST] Sleep command result: OK
../sl_radio_hal_test_runner.c:75:test_sx126x_hal_write_RealSleepCommand_ShouldPutRadioToSleep:PASS

[HW TEST] Testing sx126x_hal_read with real hardware...
[HW TEST] Status read result: OK
[HW TEST] Status value: 0xAA
../sl_radio_hal_test_runner.c:78:test_sx126x_hal_read_WithRealHardware_ShouldReceiveData:PASS

[HW TEST] Testing sx126x_hal_read radio version...
[HW TEST] Version read result: OK
[HW TEST] Version register: 0xAA
../sl_radio_hal_test_runner.c:79:test_sx126x_hal_read_ReadRadioVersion_ShouldReturnValidData:PASS

[HW TEST] Testing sx126x_hal_reset with real hardware...
[HW TEST] Reset result: OK
[HW TEST] Post-reset status read: OK (0xAA)
../sl_radio_hal_test_runner.c:82:test_sx126x_hal_reset_WithRealHardware_ShouldResetRadio:PASS

[HW TEST] Testing sx126x_hal_wakeup with real hardware...
[HW TEST] Wakeup result: OK
[HW TEST] Post-wakeup status read: OK (0xAA)
../sl_radio_hal_test_runner.c:85:test_sx126x_hal_wakeup_WithRealHardware_ShouldWakeRadio:PASS

-----------------------
13 Tests 0 Failures 0 Ignored
OK

============================================================
Test Results Summary
============================================================
ALL TESTS PASSED!
Status: SUCCESS
============================================================

==================================================
Test Results Summary
==================================================
ALL TESTS PASSED!
Status: SUCCESS
```
