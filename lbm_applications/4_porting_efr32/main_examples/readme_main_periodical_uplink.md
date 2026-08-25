# Periodical Uplink Example

This example joins a LoRaWAN network (OTAA) and sends periodic uplink messages using LoRa Basics Modem on Silicon Labs EFR32 with SX1262.

## Overview

- Joins the network with configurable credentials
- Sends a 32-bit counter uplink every **10 seconds** (configurable)
- User button triggers an immediate uplink
- Receives and logs downlinks

## Requirements

See [Hardware](../README.md#hardware) and [Software requirements](../README.md#software-requirements) in the EFR32 porting README.

Also needed for this example: LoRaWAN gateway and network server, USB for programming/debug.

## Features

- LoRaWAN 1.0.4 / OTAA
- Dual uplink ports:
  - Port 101: periodic uplinks
  - Port 102: button-triggered uplinks
- Debug tracing on the console

## Configuration

### Timing

| Parameter | Default | Macro |
|---|---|---|
| Periodic uplink interval | 10 s | `PERIODICAL_UPLINK_DELAY_S` |
| First message after join | 10 s | `DELAY_FIRST_MSG_AFTER_JOIN` |

### Credentials and region

Edit [example_options.h](example_options.h):

- `USER_LORAWAN_DEVICE_EUI` (8 bytes)
- `USER_LORAWAN_JOIN_EUI` (8 bytes)
- `USER_LORAWAN_GEN_APP_KEY` (16 bytes)
- `USER_LORAWAN_APP_KEY` (16 bytes)
- `MODEM_EXAMPLE_REGION` (default `SMTC_MODEM_REGION_EU_868`)

## Prerequisites

1. **Gateway** — e.g. Dragino LPS8N ([user manual](https://wiki.dragino.com/xwiki/bin/view/Main/User%20Manual%20for%20All%20Gateway%20models/LPS8N%20-%20LoRaWAN%20Gateway%20User%20Manual)).
2. **Network server** — e.g. ChirpStack v4 ([docs](https://www.chirpstack.io/docs/)).
3. **Provision the device** with DevEUI, JoinEUI, GenAppKey, and AppKey ([ChirpStack notes](https://wiki.dragino.com/xwiki/bin/view/Main/Notes%20for%20ChirpStack/#H1.A0Introduction)).

## How to Use

### 1. Configure credentials

Update `USER_LORAWAN_*` and `MODEM_EXAMPLE_REGION` in [example_options.h](example_options.h).

### 2. Build and flash

From `lbm_applications/4_porting_efr32`:

```bash
make sx1262 BOARD=brd4405a MODEM_APP=PERIODICAL_UPLINK
```

Firmware: `build_sx1262_<board>/app_sx1262.hex`

Other boards: `BOARD=brd4400c`, `BOARD=brd4187c`, or `BOARD=xiao_mg24`.

### 3. Run

1. Power on — device starts and attempts join
2. After join, periodic uplinks every 10 s on port 101
3. Press user button (BTN0) for an immediate uplink on port 102
4. Queue downlinks from the network server to exercise RX

## Debug Console Output

### Network Join Sequence

```
[I] INFO: Modem Initialization
[I] INFO: Use soft secure element for cryptographic functionalities
[I] WARN: No valid DevNonce in NVM, use default (0)
[I] WARN: No valid lr1mac context --> Factory reset
[I] stack_id 0
[I]  Region = EU868
[D] INFO: Event received: RESET
[I] INFO: smtc_modem_join_network
[I] Start a new join sequence in 5 seconds on stack 0
[D] INFO: Periodical uplink example is starting
...
[D] INFO: Event received: JOINED
[D] INFO: Modem is now joined
[I] INFO: add send task
[I] User LoRaWAN tx on FPort 101
```

### Manual Uplink (Button)

```
[D] INFO: Button pushed
[I] add send task
[I] User LoRaWAN tx on FPort 102
...
[D] INFO: Event received: TXDONE
[D] INFO: Transmission done
```

### Downlink Reception

```
[D] INFO: Event received: DOWNDATA
[D] Data received on port 1
[D] Received payload - (1 bytes):
[D]  10[D]
```

## Troubleshooting

- **Join fails**: match credentials and region with the network server; check gateway range/config
- **No TX**: antenna, regional frequencies, duty cycle
- **No console**: USB serial, typically 115200 baud
