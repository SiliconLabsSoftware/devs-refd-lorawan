# LBM HAL Unit Tests

This directory contains unit tests for the LoRa Basic Modem Hardware Abstraction Layer (HAL) components.

## Overview

The unit tests are organized into separate modules for each HAL peripheral:

- **Watchdog Tests** (`test_sl_lbm_hal_watchdog/`)
- **RTC Tests** (`test_sl_lbm_hal_rtc/`)
- **LP Timer Tests** (`test_sl_lbm_hal_lp_timer/`)
- **RNG Tests (PSA)** (`test_sl_lbm_hal_rng/`)
- **Context Management Tests (MSC)** (`test_sl_lbm_hal_context/`)
- **GPIO Tests** (`test_sl_lbm_hal_gpio/`)

## Test Framework

The tests use the [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity) which is included in the Silicon Labs Simplicity SDK.

## Directory Structure

```
test/
├── sl_lbm_hal_test_runner.h          # Main test runner header
├── sl_lbm_hal_test_runner.c          # Main test runner implementation
├── main_test_example.c               # Example integration
├── README.md                         # This file
├── test_sl_lbm_hal_watchdog/
│   ├── test_sl_lbm_hal_watchdog.h    # Watchdog test header
│   └── test_sl_lbm_hal_watchdog.c    # Watchdog test implementation
├── test_sl_lbm_hal_rtc/
│   ├── test_sl_lbm_hal_rtc.h         # RTC test header
│   └── test_sl_lbm_hal_rtc.c         # RTC test implementation
├── test_sl_lbm_hal_lp_timer/
│   ├── test_sl_lbm_hal_lp_timer.h    # LP Timer test header
│   └── test_sl_lbm_hal_lp_timer.c    # LP Timer test implementation
├── test_sl_lbm_hal_rng/
│   ├── test_sl_lbm_hal_rng.h         # RNG test header
│   └── test_sl_lbm_hal_rng.c         # RNG test implementation
├── test_sl_lbm_hal_context/
│   ├── test_sl_lbm_hal_context.h     # Context management test header
│   └── test_sl_lbm_hal_context.c     # Context management test implementation
├── test_sl_lbm_hal_iadc/
│   ├── test_sl_lbm_hal_iadc.h        # IADC test header
│   └── test_sl_lbm_hal_iadc.c        # IADC test implementation
└── test_sl_lbm_hal_gpio/
    ├── test_sl_lbm_hal_gpio.h        # GPIO test header
    └── test_sl_lbm_hal_gpio.c        # GPIO test implementation
```

## Running Tests

### Option 1: Using the Test Runner Functions

Include the test runner header and call the appropriate function:

```c
#include "sl_lbm_hal_test_runner.h"

void sl_main_porting_tests_init(void) {
  // Run all HAL tests
  sl_lbm_hal_run_all_tests();
}

void sl_main_porting_tests_process_action(void) {
  // Reload WDOG counter to prevent MCU reset during test process.
  sl_lbm_hal_watchdog_reload();
}
```

### Option 2: Individual Test Functions

You can also run tests for individual peripherals:

```c
#include "test_sl_lbm_hal_watchdog/test_sl_lbm_hal_watchdog.h"
#include "test_sl_lbm_hal_rtc/test_sl_lbm_hal_rtc.h"
#include "test_sl_lbm_hal_lp_timer/test_sl_lbm_hal_lp_timer.h"
#include "test_sl_lbm_hal_gpio/test_sl_lbm_hal_gpio.h"

void sl_main_porting_tests_init(void) {
    int watchdog_failures = sl_lbm_hal_run_watchdog_test();
    int rtc_failures = sl_lbm_hal_run_rtc_test();
    int lp_timer_failures = sl_lbm_hal_run_lp_timer_test();
    int gpio_failures = sl_lbm_hal_run_gpio_test();

    int test_failures = watchdog_failures + rtc_failures + lp_timer_failures + gpio_failures;
}
```
## Test Coverage

### Watchdog Tests
The watchdog tests follow the approach of initializing first, then verifying correct operation:

1. `test_sl_lbm_hal_watchdog_initialization()` - Initializes watchdog and verifies completion
2. `test_sl_lbm_hal_watchdog_reload_functionality()` - Tests reload functionality works
3. `test_sl_lbm_hal_watchdog_multiple_reloads()` - Tests multiple consecutive reloads

### RTC Tests
The RTC tests follow the same approach of initialization followed by operational verification:

1. `test_sl_lbm_hal_rtc_initialization()` - Initializes RTC and verifies completion
2. `test_sl_lbm_hal_rtc_is_running_after_init()` - Verifies RTC is running after init
3. `test_sl_lbm_hal_rtc_time_functions_operational()` - Tests time reading functions work correctly
4. `test_sl_lbm_hal_rtc_wakeup_timer_operational()` - Tests wakeup timer functions work
5. `test_sl_lbm_hal_rtc_time_consistency_check()` - Tests consistency between time formats
6. `test_sl_lbm_hal_rtc_wakeup_timer_control()` - Tests timer start/stop control
7. `test_sl_lbm_hal_rtc_wrapping_functionality()` - Tests time wrapping functionality

### LP Timer Tests
The LP Timer tests verify Low Power Timer functionality including timing accuracy and callback execution:

1. `test_sl_lbm_hal_lp_timer_initialization()` - Initializes LP timer and verifies LETIMER0 configuration
2. `test_sl_lbm_hal_lp_timer_start_stop()` - Tests timer start/stop functionality
3. `test_sl_lbm_hal_lp_timer_irq_enable_disable()` - Tests interrupt enable/disable operations
4. `test_sl_lbm_hal_lp_timer_callback_execution()` - Tests callback functionality with context passing
5. `test_sl_lbm_hal_lp_timer_multiple_starts()` - Tests multiple timer start behavior (override)
6. `test_sl_lbm_hal_lp_timer_timing_accuracy()` - Verifies basic timing accuracy

### GPIO Tests
The GPIO tests verify Hardware Abstraction Layer functionality for GPIO pin control and interrupt handling:

1. `test_sl_hal_gpio_init_out_basic()` - Tests basic GPIO output initialization with different initial values
2. `test_sl_hal_gpio_init_out_boundary_conditions()` - Tests output initialization with valid/invalid pin numbers
3. `test_sl_hal_gpio_set_get_value_basic()` - Tests GPIO set/get value operations for output pins
4. `test_sl_hal_gpio_set_get_value_boundary_conditions()` - Tests set/get operations with valid/invalid pins
5. `test_sl_hal_gpio_external_interrupt_pb01()` - **Optional** external interrupt test with PB01 button (macro-controlled)

**External Interrupt Test Features:**
- Controlled by `ENABLE_EXTERNAL_INTERRUPT_TEST` macro (set to 1 to enable)
- Tests real hardware interrupt generation using PB01 button
- 5-second timeout for manual button press interaction
- Validates interrupt callback execution and counting

### RNG Tests (PSA Crypto)
The RNG tests verify Random Number Generation using PSA Crypto APIs

1. `test_sl_lbm_hal_rng_initialization()` - Tests PSA Crypto initialization
2. `test_sl_lbm_hal_rng_get_random_basic()` - Tests basic random number generation functionality
3. `test_sl_lbm_hal_rng_get_random_uniqueness()` - Tests that multiple calls produce different random values
4. `test_sl_lbm_hal_rng_get_random_in_range_basic()` - Tests random number generation within specified ranges
5. `test_sl_lbm_hal_rng_get_random_in_range_bounds()` - Tests boundary conditions for range-based random generation
6. `test_sl_lbm_hal_rng_get_random_in_range_reversed_params()` - Tests error handling when min > max parameters
7. `test_sl_lbm_hal_rng_get_random_in_range_same_values()` - Tests behavior when min == max (should return that value)
8. `test_sl_lbm_hal_rng_multiple_calls_stability()` - Tests system stability across multiple consecutive random calls

### Context Management Tests (NVM3)
The context management tests verify non-volatile memory operations using NVM3:

1. `test_sl_lbm_hal_context_store_basic()` - Tests basic store operations for different context types
2. `test_sl_lbm_hal_context_restore_uninitialized()` - Tests restore behavior when no data exists
3. `test_sl_lbm_hal_context_offset_handling()` - Tests offset-based operations for multi-instance contexts
4. `test_sl_lbm_hal_context_boundary_conditions()` - Tests boundary conditions and error handling
5. `test_sl_lbm_hal_context_flash_pages_erase_basic()` - Tests flash page erase functionality
6. `test_sl_lbm_hal_context_flash_pages_erase_boundary_conditions()` - Tests erase boundary conditions

### Crashlog Tests
The crashlog tests verify crash logging and recovery functionality:

1. `test_sl_lbm_hal_crashlog_store_restore_basic()` - Tests basic store and restore operations with data integrity verification
2. `test_sl_lbm_hal_crashlog_store_max_size()` - Tests storing maximum size data
3. `test_sl_lbm_hal_crashlog_store_oversized()` - Tests handling of oversized data
4. `test_sl_lbm_hal_crashlog_restore_empty()` - Tests restore behavior when crashlog is empty or cleared
5. `test_sl_lbm_hal_crashlog_store_overwrite()` - Tests that new crashlog entries overwrite previous ones
6. `test_sl_lbm_hal_crashlog_edge_cases()` - Tests edge cases including single character data and embedded null characters

### IADC Tests
The IADC tests verify initialization and battery level measurement functionality:

1. `test_sl_lbm_hal_init_iadc0_basic()` - Tests basic IADC initialization
2. `test_sl_lbm_hal_get_battery_level()` - Tests battery level measurement functionality
3. `test_sl_lbm_hal_iadc_initialization_verification()` - Verifies IADC initialization and operation

## Test Philosophy

The tests are designed with the philosophy that:

1. **Initialization First**: Each peripheral must be properly initialized before testing
2. **Operational Verification**: After initialization, verify the peripheral is running correctly
3. **Functional Testing**: Test that all functions work as expected on the running peripheral
4. **State Persistence**: Verify that the peripheral maintains correct state throughout operations
5. **No Repeated Initialization**: Once initialized, peripherals should remain operational for subsequent tests

## Customization

You can customize the tests by:

1. Adding new test cases to the existing test files
2. Creating additional test modules for other HAL components
3. Modifying test tolerances and timeouts in the test files
4. Adding integration tests that test multiple components together

## Example Output

When tests run successfully, you'll see output like:

```
=== Running LBM HAL Unit Tests ===

--- Running Watchdog Tests ---
test_sl_lbm_hal_watchdog_initialization:PASS
test_sl_lbm_hal_watchdog_is_running_after_init:PASS
test_sl_lbm_hal_watchdog_reload_functionality:PASS
test_sl_lbm_hal_watchdog_multiple_reloads:PASS
test_sl_lbm_hal_watchdog_state_verification:PASS

--- Running RTC Tests ---
test_sl_lbm_hal_rtc_initialization:PASS
test_sl_lbm_hal_rtc_is_running_after_init:PASS
test_sl_lbm_hal_rtc_time_functions_operational:PASS
test_sl_lbm_hal_rtc_wakeup_timer_operational:PASS
test_sl_lbm_hal_rtc_time_consistency_check:PASS
test_sl_lbm_hal_rtc_wakeup_timer_control:PASS
test_sl_lbm_hal_rtc_wrapping_functionality:PASS

--- Running LP Timer Tests ---
test_sl_lbm_hal_lp_timer_initialization:PASS
test_sl_lbm_hal_lp_timer_invalid_id:PASS
test_sl_lbm_hal_lp_timer_start_stop:PASS
test_sl_lbm_hal_lp_timer_irq_enable_disable:PASS
test_sl_lbm_hal_lp_timer_callback_execution:PASS
test_sl_lbm_hal_lp_timer_multiple_starts:PASS
test_sl_lbm_hal_lp_timer_timing_accuracy:PASS

--- Running GPIO Tests ---
test_sl_hal_gpio_init_out_basic:PASS
test_sl_hal_gpio_init_out_boundary_conditions:PASS
test_sl_hal_gpio_set_get_value_basic:PASS
test_sl_hal_gpio_set_get_value_boundary_conditions:PASS

--- Running External Interrupt Test (PB01 Button) ---
Please press the button on PB01 within 5 seconds...
✓ External interrupt triggered successfully!
Interrupt count: 1
test_sl_hal_gpio_external_interrupt_pb01:PASS

=== Test Summary ===
All LBM HAL tests PASSED!
```

## Notes

- Tests are designed to be run on the target hardware or with appropriate mocking
- Some tests may require specific timing or hardware conditions
- For production builds, consider excluding test files to reduce memory footprint
- The tests can be integrated into CI/CD pipelines for automated validation
- Change Sleeptimer software component clock source to BURTC before build images.
