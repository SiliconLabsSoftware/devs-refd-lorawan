# Implementation reference 4: Porting on Silicon Labs EFR32

This folder contains a LoRa Basics Modem (LBM) port for Silicon Labs EFR32 devices with Semtech SX1262 radio, plus application examples.

## Hardware

<a id="hardware"></a>

| Board | MCU | Radio |
|---|---|---|
| `brd4405a` (default) | EFR32FG28B320F1024IM48 | On-board SX1262 + SP3T RF switch |
| `brd4400c` | EFR32ZG28B322 (xG28 Pro Kit) | SX1262 via BRD8042A adapter |
| `brd4187c` | EFR32MG24 (xG24 Pro Kit) | SX1262 via BRD8042A adapter |
| `xiao_mg24` | EFR32MG24 (Seeed XIAO MG24) | Wio-SX1262 shield |

Radio target for all boards: **sx1262**.

## Software requirements

<a id="software-requirements"></a>

- **GNU Arm Embedded Toolchain** (see root [README.md](../../README.md))
- **Make**
- **Silicon Labs Simplicity SDK 2026.6.1** (vendored under [mcu_drivers/simplicity_sdk_2026.6.1](mcu_drivers/simplicity_sdk_2026.6.1))
- Optional: Silicon Labs Simplicity Commander for flashing

## Getting Started

From this directory, run `make help` for build targets and options.

Compilation options can be provided on the command line or in [app_makefiles/app_options.mk](app_makefiles/app_options.mk).

Default board: `BOARD=brd4405a`.

### Common options

| Option | Description | Default |
|---|---|---|
| `BOARD` | `brd4405a`, `brd4400c`, `brd4187c`, `xiao_mg24` | `brd4405a` |
| `MODEM_APP` | `PERIODICAL_UPLINK`, `PORTING_TESTS`, `LCTT_CERTIF`, `PING_PONG` | (required) |
| `ALLOW_FUOTA` | Enable FUOTA build (`yes` / `no`) | `no` |
| `FUOTA_VERSION` | FUOTA package version when `ALLOW_FUOTA=yes` | `1` |
| `CRYPTO` | LBM crypto backend — must be `SOFT` for SX1262 (see [Crypto](#crypto-hardware-accelerated-on-efr32-series-2)) | `SOFT` |
| `CRYPTO_HW_OVERRIDE` | App-level override: `EFR32` replaces soft SE with Series 2 hardware crypto | `EFR32` |
| `LBM_TRACE` / `APP_TRACE` | Modem / app traces | `yes` |
| `PING_PONG_ROLE` | Required for `xiao_mg24` + `PING_PONG`: `PING` or `PONG` | — |

Build output:

```text
build_sx1262_<board>/app_sx1262.hex
```

### Main Examples

#### Periodical Uplink

Joins a LoRaWAN network and sends uplinks periodically (default every 10 s), or when the user button is pressed.

LoRaWAN credentials and region are in [main_examples/example_options.h](main_examples/example_options.h) (`USER_LORAWAN_*`, `MODEM_EXAMPLE_REGION`, default `EU_868`).

See [main_examples/readme_main_periodical_uplink.md](main_examples/readme_main_periodical_uplink.md).

```bash
make sx1262 BOARD=brd4405a MODEM_APP=PERIODICAL_UPLINK
```

#### Porting tests

Automatic HAL test suite (SPI, radio IRQ, time, timer, random, radio config, sleep / low power).

See [main_examples/readme_main_porting_tests.md](main_examples/readme_main_porting_tests.md).

```bash
make sx1262 BOARD=brd4405a MODEM_APP=PORTING_TESTS
```

#### LCTT Certification

Application for the LoRaWAN Certification Test Tool (LCTT). Push BTN0 to enable/disable certification mode (port 224).

Credentials and region: [main_examples/example_options.h](main_examples/example_options.h).

See [main_examples/readme_main_lctt_certif.md](main_examples/readme_main_lctt_certif.md).

```bash
make sx1262 BOARD=brd4405a MODEM_APP=LCTT_CERTIF
```

#### Ping Pong

Direct LoRa ping-pong between two devices (no gateway). Radio parameters are defined in `main_ping_pong.c`.

- Pro Kit / `brd4405a`: select role at runtime with **BTN0** (PING) / **BTN1** (PONG)
- `xiao_mg24`: set `PING_PONG_ROLE=PING` or `PONG` at build time

See [main_examples/readme_main_ping_pong.md](main_examples/readme_main_ping_pong.md).

```bash
make sx1262 BOARD=brd4405a MODEM_APP=PING_PONG
```

For XIAO MG24:

```bash
make sx1262 BOARD=xiao_mg24 MODEM_APP=PING_PONG PING_PONG_ROLE=PING
make clean_all
make sx1262 BOARD=xiao_mg24 MODEM_APP=PING_PONG PING_PONG_ROLE=PONG
```

### FUOTA

Enable FUOTA on the periodical uplink example with `ALLOW_FUOTA=yes`. Fragment limits and `FUOTA_VERSION` are in [app_makefiles/app_options.mk](app_makefiles/app_options.mk).

See [main_examples/readme_fuota_feature.md](main_examples/readme_fuota_feature.md).

```bash
make sx1262 BOARD=brd4405a MODEM_APP=PERIODICAL_UPLINK ALLOW_FUOTA=yes
```

## MCU Porting

MCU-specific code:

- [mcu_drivers](mcu_drivers): Simplicity SDK 2026.6.1 and board support under `mcu_drivers/board/`
- [smtc_hal_efr32](smtc_hal_efr32): EFR32 helpers used by the modem HAL (GPIO, SPI, UART, time, WDOG, …)

## Radio Porting

Radio HAL / BSP for SX1262: [radio_hal](radio_hal)

## Modem Porting

Modem HAL: [smtc_modem_hal/smtc_modem_hal.c](smtc_modem_hal/smtc_modem_hal.c)

## Crypto (hardware-accelerated on EFR32 Series 2)

Even though the build uses `CRYPTO=SOFT`, LoRaWAN crypto on this port is **not** plain software AES on the CPU.

| Flag | Role |
|---|---|
| `CRYPTO=SOFT` | Required LBM library option for SX1262. SX1262 has no crypto engine, so LBM cannot use `CRYPTO=LR11XX`. The soft secure-element sources are compiled into `lbm_lib`. |
| `CRYPTO_HW_OVERRIDE=EFR32` | EFR32 app option. Compiles [smtc_modem_crypto/efr32_secure_element](smtc_modem_crypto/efr32_secure_element) and linker-wraps `smtc_secure_element_*` so calls go to Silicon Labs SE Manager (Series 2 hardware crypto) instead of the soft SE. |

**Result:** LBM still builds with the soft SE API (`CRYPTO=SOFT`), but at link time those APIs are overridden. AES / CMAC used by join, MIC, and payload encryption run on the **EFR32 Series 2 Secure Element** hardware.

To fall back to true software crypto, clear or unset `CRYPTO_HW_OVERRIDE` in [app_makefiles/app_options.mk](app_makefiles/app_options.mk). Do **not** set `CRYPTO=LR11XX` for this SX1262 port — that backend only applies to LR11xx radios.
