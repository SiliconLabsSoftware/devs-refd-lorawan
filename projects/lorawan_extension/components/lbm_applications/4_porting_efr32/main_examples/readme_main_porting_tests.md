# Main Porting Tests Example

## Overview

The **main_porting_tests** example is a comprehensive test suite designed to verify and validate the porting of LoRaWAN Modem HAL (Hardware Abstraction Layer) on Silicon Labs EFR32 platform. This example tests the basic functionality of the hardware abstraction layer to ensure that the porting is implemented correctly.

## Purpose

- **Verify correctness of HAL implementation**
- **Validate basic hardware functionality**
- **Ensure timing and performance requirements**
- **Detect errors in the porting process**

## Hardware Requirements

- **Silicon Labs EFR32 series MCU**
- **LoRa radio module** (SX126X)

## Files Structure

```
main_porting_tests/
├── README.md                    # This file
├── main_porting_tests.c      # Main test implementation
├── main_porting_tests.h      # Header file
├── main.c                      # Application entry point
└── app.c                       # Application callbacks
```

## Tests Performed

### 1. **SPI Communication Test** (`porting_test_spi`)
- Tests SPI communication with radio module
- Reads firmware version from radio chip (SX126X/LR11XX)
- Validates SPI driver implementation correctness

### 2. **Radio IRQ Test** (`porting_test_radio_irq`)
- Tests GPIO interrupt configuration
- Tests radio timeout interrupt handling
- Validates interrupt callback mechanism

### 3. **Time Management Tests** (`porting_test_get_time`)
- **Get time in seconds**: Tests `smtc_modem_hal_get_time_in_s()`
- **Get time in milliseconds**: Tests `smtc_modem_hal_get_time_in_ms()`
- Verifies RTC implementation accuracy

### 4. **Timer IRQ Tests**
- **Basic Timer IRQ** (`porting_test_timer_irq`): Tests timer interrupt functionality
- **Stop Timer** (`porting_test_stop_timer`): Verifies timer stop mechanism
- **Low Power Timer** (`porting_test_timer_irq_low_power`): Tests timer in sleep mode

### 5. **Interrupt Control Test** (`porting_test_disable_enable_irq`)
- Tests enable/disable interrupt functionality
- Verifies interrupt masking mechanism

### 6. **Random Number Generator Test** (`porting_test_random`)
- Verifies random number generator quality
- Tests distribution and randomness
- Validates range-based random generation

### 7. **Radio Configuration Tests**
- **RX Configuration** (`porting_test_config_rx_radio`): Tests radio configuration time for reception
- **TX Configuration** (`porting_test_config_tx_radio`): Tests radio configuration time for transmission

### 8. **Sleep Mode Test** (`porting_test_sleep_ms`)
- Verifies low power sleep implementation
- Validates sleep timing accuracy

### 9. **Flash Storage Test** (`porting_test_flash`) - Optional
- Tests context store/restore functionality
- Verifies flash memory operations (if enabled)

## Configuration Options

### Compile-time Options

```c
#define ENABLE_TEST_FLASH 0    // Enable/disable flash tests
#define NB_LOOP_TEST_SPI 2     // Number of SPI test iterations
#define NB_LOOP_TEST_CONFIG_RADIO 2  // Radio config test loops
```

### Timing Margins

```c
#define MARGIN_GET_TIME_IN_MS 1          // ±1ms margin for timing tests
#define MARGIN_TIMER_IRQ_IN_MS 2         // +2ms margin for timer IRQ
#define MARGIN_TIME_CONFIG_RADIO_IN_MS 8 // Max 8ms for radio config
#define MARGIN_SLEEP_IN_MS 2             // ±2ms margin for sleep timing
```

## How to Run the Example
Use Simplicity Studio to Build and Run

### 1. Build and Flash

* Add extension to Simplicity SDK. Please refer [How to use LoRaWAN extension on Simplicity Studio 5](../../../../../README.md#how-to-use-lorawan-extension-on-simplicity-studio-5)
* Create project Main Porting Test
* Build and flash FW for device

### 2. Monitor Output
* Use Launch Console to view output

## Test Output Example
* if #define ENABLE_TEST_FLASH 0

```
PORTING_TEST example is starting

[D] ----------------------------------------
 porting_test_spi : [D]  OK
[D] ----------------------------------------
 porting_test_radio_irq : [D]  OK
[D] ----------------------------------------
 porting_test_get_time :
[D]  * Get time in second: [D]  OK
[D] Time expected 5s / get 5s (no margin)
[D]  * Get time in millisecond: [D]  OK
[D]  Time expected 2031ms / get 2031ms (margin +/-1ms)
[D] ----------------------------------------
 porting_test_timer_irq : [D]  OK
[D] Timer irq configured with 3000ms / get 3000ms (margin +2ms)
[D] ----------------------------------------
 porting_test_stop_timer : [D]  OK
[D] ----------------------------------------
 porting_test_disable_enable_irq : [D]  OK
[D] ----------------------------------------
 porting_test_random :
[D]  * Get random nb : [D]  OK
[D]  random1 = 3080908166, random2 = 1571195643
[D]  * Get random nb in range : [D]  OK
[D]  random1 = 3, random2 = 4 in range [1;42]
[D]  * Get random draw : [D]  OK
[D]  Random draw of 100000 numbers between [1;10] range
[D] ----------------------------------------
 porting_test_config_rx_radio :[D]  OK
[D] ----------------------------------------
 porting_test_config_tx_radio :[D]  OK
[D] ----------------------------------------
 porting_test_sleep_ms :[D]  OK
[D] Sleep time expected 2000ms / get 2000ms (margin +/-2ms)
[D] ----------------------------------------
 porting_test_timer_irq_low_power : [D]  OK
[D] Timer irq configured with 3000ms / get 3000ms (margin +2ms)
[D] ----------------------------------------
END
```

* if #define ENABLE_TEST_FLASH 1

```
PORTING_TEST example is starting

[D] ----------------------------------------
 porting_test_flash :
[D]  !! TEST TO BE LAUNCH TWICE !! To check writing after MCU reset
[D]
 * Context LORAWAN_STACK :
[D]  Read:  { [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D] , [D] 9[D]  }
[D]  Write: { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Read:  { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Store/restore without MCU reset :[D]  OK
[D]
 * Context MODEM :
[D]  Read:  { [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D] , [D] 9[D]  }
[D]  Write: { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Read:  { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Store/restore without MCU reset :[D]  OK
[D]
 * Context KEY_MODEM :
[D]  Read:  { [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D] , [D] 9[D]  }
[D]  Write: { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Read:  { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Store/restore without MCU reset :[D]  OK
[D]
 * Context SECURE_ELEMENT :
[D]  Read:  { [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D] , [D] 9[D]  }
[D]  Write: { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Read:  { [D] 1[D] , [D] 2[D] , [D] 3[D] , [D] 4[D] , [D] 5[D] , [D] 6[D] , [D] 7[D] , [D] 8[D]  }
[D]  Store/restore without MCU reset :[D]  OK
[D]
 MCU RESET => relaunch tests and check if read after reset = write before reset

```
