# Periodical Uplink Example

This example demonstrates a LoRaWAN end-device implementation that performs periodic uplink transmissions using the Semtech LoRaWAN Basic Modem library integrated with Silicon Labs Simplicity SDK.

## Overview

- **End Device**: Joins a LoRaWAN network and sends periodic uplink messages containing a 32-bit counter
- **Network Joining**: Uses OTAA (Over-The-Air Activation) with configurable credentials
- **Periodic Transmission**: Automatically sends uplink messages every 60 seconds (configurable)
- **Manual Transmission**: User button press triggers immediate uplink transmission
- **Downlink Support**: Receives and processes downlink messages from the network server

## Requirements

### Hardware
- Silicon Labs WSTK BRD4002A
- Silicon Labs EFR32xG28 radio board
- Semtech SX1262 evaluation board
- Silicon Labs BRD8042A adapter board
- LoRaWAN Gateway and Network Server
- USB cable for programming and debug

### Software
- Silicon Labs Simplicity SDK 2025.6.0
- Silicon Labs Simplicity Commander tool
- LoRaWAN extension

## Features

- **LoRaWAN 1.0.4 Compliance**: Full LoRaWAN stack implementation
- **OTAA Join Procedure**: Secure network joining with DevEUI, JoinEUI, and AppKey
- **Dual Uplink Ports**:
  - Port 101: Periodic automatic uplinks
  - Port 102: Manual button-triggered uplinks
- **Downlink Reception**: Processes received downlink data and metadata
- **Debug Tracing**: Comprehensive debug output via console

## Configuration Parameters

### Timing Configuration
- **Periodic Uplink Interval**: 60 seconds (configurable via `PERIODICAL_UPLINK_DELAY_S`)
- **First Message Delay**: 60 seconds after join (configurable via `DELAY_FIRST_MSG_AFTER_JOIN`)
- **Watchdog Reload Period**: 20 seconds

### LoRaWAN Credentials
Configure in `sl_lorawan_user_credential.h`:
- `SL_USER_LORAWAN_DEVICE_EUI`: Device EUI (8 bytes)
- `SL_USER_LORAWAN_JOIN_EUI`: Join EUI/App EUI (8 bytes)
- `SL_USER_LORAWAN_GEN_APP_KEY`: Generic App Key (16 bytes)
- `SL_USER_LORAWAN_APP_KEY`: App Key/Network Key (16 bytes)

### Regional Settings
- Default region configured via `SL_MODEM_EXAMPLE_REGION`
- Supports EU868, US915, AS923, AU915, CN470, and other regions

## Prerequisites

### 1. Setup LoRaWAN Gateway
The LoRaWAN gateway used in this example is the Dragino LPS8N. See [LPS8N -- LoRaWAN Gateway User Manual](https://wiki.dragino.com/xwiki/bin/view/Main/User%20Manual%20for%20All%20Gateway%20models/LPS8N%20-%20LoRaWAN%20Gateway%20User%20Manual) for more information.

### 2. Setup LoRaWAN Network Server
ChirpStack Network Server is used for this example. See the [ChirpStack Documentation v4](https://www.chirpstack.io/docs/) for more information.

### 3. Add Device to Network Server
Follow the [Notes for ChirpStack](https://wiki.dragino.com/xwiki/bin/view/Main/Notes%20for%20ChirpStack/#H1.A0Introduction) to add a device with the following credentials:
- Device EUI
- Join EUI (Application EUI)
- Generic Application Key
- Application Key

## How to Use

### 1. Configure Credentials
Modify the [LoRaWAN Credentials](#lorawan-credentials) in the `example_options.h` file with the EUI and keys created in your network server.

### 2. Build and Flash
Build the project for your target hardware and flash the firmware to your device.

### 3. Configure Downlink Testing
Downlink data can be sent from the ChirpStack Network Server. Received data will appear in the debug console output.

### 4. Device Operation
1. **Power On**: Device automatically starts and attempts to join the network
2. **Join Process**: Monitor the debug console for join status messages
3. **Automatic Uplinks**: After successful join, device sends periodic uplinks every 60 seconds
4. **Manual Uplinks**: Press user button (BTN0) to send immediate uplink on port 102
5. **Monitor**: Observe debug console for transmission status and received downlinks

## Debug Console Output

### Network Join Sequence
```
[I] INFO: Modem Initialization
[I] INFO: Use soft secure element for cryptographic functionalities
[I] WARN: No valid DevNonce in NVM, use default (0)
[I] WARN: No valid lr1mac context --> Factory reset
[I] stack_id 0
[I]  DevNonce = 0
[I]  JoinNonce = 0xff ff ff, NetID = 0xff ff ff
[I]  Region = EU868
[I] LoRaWAN Certification is disabled on stack 0
[D] Modem event callback
[D] INFO: Event received: RESET
[I] INFO: smtc_modem_join_network
[I] Start a new join sequence in 5 seconds on stack 0
[D] INFO: Periodical uplink example is starting
[I] DevEUI - (8 bytes):
[I]  00[I]  00[I]  00[I]  00[I]  00[I]  00[I]  00[I]  20[I]
[I] JoinEUI - (8 bytes):
[I]  00[I]  00[I]  00[I]  00[I]  00[I]  00[I]  00[I]  20[I]
[I] DevNonce 0x1, stack_id 0
[I]
  *************************************
  * [I] Send Payload  for stack_id = 0[I]
  *************************************
[I]   Tx  LoRa at 9042 ms: freq:868300000, SF7, BW125, len 23 bytes 14 dBm, fcnt_up 0, toa = 62
[I]
  *************************************
  * [I]  TX DONE[I]
  *************************************
[I]
  Open RX1 for Hook Id = 1[I]   RX1 LoRa at 14101 ms: freq:868300000, SF7, BW125, sync word = 0x34
[I]   Timer will expire in 4968 ms
[I]
  *************************************
  * [I] Receive a Valid downlink RX1 for stack_id = 0[I]
  *************************************
[I]  update join procedure
[I] MacTxFrequency [0] = 867100000
[I] MacDataRateChannel [0] = [I]  1[I]  1[I]  1[I]  1[I]  1[I]  1[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  
[I] MacChannelIndexEnabled [0] = 1
[I] MacTxFrequency [1] = 867300000
[I] MacDataRateChannel [1] = [I]  1[I]  1[I]  1[I]  1[I]  1[I]  1[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  
[I] MacChannelIndexEnabled [1] = 1
[I] MacTxFrequency [2] = 867500000
[I] MacDataRateChannel [2] = [I]  1[I]  1[I]  1[I]  1[I]  1[I]  1[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  
[I] MacChannelIndexEnabled [2] = 1
[I] MacTxFrequency [3] = 867700000
[I] MacDataRateChannel [3] = [I]  1[I]  1[I]  1[I]  1[I]  1[I]  1[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  
[I] MacChannelIndexEnabled [3] = 1
[I] MacTxFrequency [4] = 867900000
[I] MacDataRateChannel [4] = [I]  1[I]  1[I]  1[I]  1[I]  1[I]  1[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  0[I]  
[I] MacChannelIndexEnabled [4] = 1
[I]  DevAddr= 4500b8
[I]  MacRx1DataRateOffset= 0
[I]  MacRx2DataRate= 0
[I]  MacRx1Delay= 1
[D] Modem event callback
[D] INFO: Event received: JOINED
[D] INFO: Modem is now joined
[I] INFO: add send task
[I] User LoRaWAN tx on FPort 101
[I]
  *************************************
  * [I] Send Payload  for stack_id = 0[I]
  *************************************
[I]   Tx  LoRa at 17851 ms: freq:868300000, SF7, BW125, len 17 bytes 14 dBm, fcnt_up 1, toa = 52
```

### Manual Uplink Transmission (Button Press)
```
[D] INFO: Button pushed
[I] add send task
[I] User LoRaWAN tx on FPort 102
[I]
  *************************************
  * [I] Send Payload  for stack_id = 0[I]
  *************************************
[I]   Tx  LoRa at 280969 ms: freq:868300000, SF7, BW125, len 17 bytes 0 dBm, fcnt_up 33, toa = 52
[I]
  *************************************
  * [I]  TX DONE[I]
  *************************************
[I]
  Open RX1 for Hook Id = 1[I]   RX1 LoRa at 282019 ms: freq:868300000, SF7, BW125, sync word = 0x34
[I]   Timer will expire in 978 ms
[I]
  *************************************
  * [I] RX1 Timeout for stack_id = 0[I]
  *************************************
[I]
  Open RX2 for Hook Id = 1[I]   RX2 LoRa at 283056 ms: freq:869525000, SF12, BW125, sync word = 0x34
[I]   Timer will expire in 999 ms
[I]
  *************************************
  * [I] RX2 Timeout for stack_id = 0[I]
  *************************************
[D] Modem event callback
[D] INFO: Event received: TXDONE
[D] INFO: Transmission done
[D] Modem event callback
[D] INFO: Event received: ALARM
[I] INFO: add send task
[I] User LoRaWAN tx on FPort 101
[I]
```

### Downlink Data Reception
```
[D] Modem event callback
[D] INFO: Event received: DOWNDATA
[D] Data received on port 1
[D] Received payload - (1 bytes):
[D]  10[D]
[D] Modem event callback
[D] INFO: Event received: TXDONE
[D] INFO: Transmission done
[D] Modem event callback
[D] INFO: Event received: ALARM
[I] INFO: add send task
```

## Troubleshooting

### Join Issues
- Verify LoRaWAN credentials match network server configuration
- Check regional settings match your gateway configuration
- Ensure gateway is within range and operational
- Verify network server device provisioning

### Transmission Problems
- Check antenna connections
- Verify regional frequency settings
- Monitor duty cycle limitations
- Check network server downlink queue

### Debug Console Issues
- Verify USB connection
- Check serial port settings (typically 115200 baud)
- Ensure debug trace is enabled in configuration
