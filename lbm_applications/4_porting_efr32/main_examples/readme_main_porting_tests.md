# Porting Tests Example

HAL / radio porting test suite for LoRa Basics Modem on Silicon Labs EFR32 + SX1262. Use it to validate SPI, IRQs, timing, timers, RNG, radio config, and sleep before running network examples.

## Purpose

- Verify HAL implementation correctness
- Validate basic MCU / radio behavior
- Check timing margins
- Catch porting mistakes early

## Requirements

See [Hardware](../README.md#hardware) and [Software requirements](../README.md#software-requirements) in the EFR32 porting README.

## Files

Shared application entry lives under `main_examples/` (not a nested folder):

```text
main_examples/
├── main.c                 # Shared entry / FreeRTOS app task
├── app.h                  # Application interface
├── main_porting_tests.c   # Test implementation
├── main_porting_tests.h   # Test API
└── readme_main_porting_tests.md
```

## Tests

1. **SPI** (`porting_test_spi`) — read radio firmware / ID over SPI
2. **Radio IRQ** (`porting_test_radio_irq`) — timeout IRQ and callback
3. **Time** (`porting_test_get_time`) — `smtc_modem_hal_get_time_in_s()` / `_ms()`
4. **Timer IRQ** — basic (`porting_test_timer_irq`), stop (`porting_test_stop_timer`), low-power (`porting_test_timer_irq_low_power`)
5. **IRQ mask** (`porting_test_disable_enable_irq`)
6. **RNG** (`porting_test_random`)
7. **Radio config** — RX (`porting_test_config_rx_radio`) / TX (`porting_test_config_tx_radio`)
8. **Sleep** (`porting_test_sleep_ms`)
9. **Flash** (`porting_test_flash`) — optional context store/restore

## Configuration

In `main_porting_tests.c`:

```c
#define ENABLE_TEST_FLASH 0
#define NB_LOOP_TEST_SPI 2
#define NB_LOOP_TEST_CONFIG_RADIO 2

#define MARGIN_GET_TIME_IN_MS 1
#define MARGIN_TIMER_IRQ_IN_MS 2
#define MARGIN_TIME_CONFIG_RADIO_IN_MS 8
#define MARGIN_SLEEP_IN_MS 2
```

When `ENABLE_TEST_FLASH` is non-zero, flash tests run and other porting tests are skipped (run twice across reset).

## Build and run

From `lbm_applications/4_porting_efr32`:

```bash
make sx1262 BOARD=brd4405a MODEM_APP=PORTING_TESTS
```

Firmware: `build_sx1262_<board>/app_sx1262.hex`

Flash the image and open a serial console (typically 115200 baud).

## Sample output

### `ENABLE_TEST_FLASH 0`

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

### `ENABLE_TEST_FLASH 1`

```
PORTING_TEST example is starting

[D] ----------------------------------------
 porting_test_flash :
[D]  !! TEST TO BE LAUNCH TWICE !! To check writing after MCU reset
...
[D]  Store/restore without MCU reset :[D]  OK
...
 MCU RESET => relaunch tests and check if read after reset = write before reset
```
