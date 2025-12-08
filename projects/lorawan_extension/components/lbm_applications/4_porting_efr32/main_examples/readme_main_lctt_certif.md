# Main LCTT Certification Example

## Overview

This example demonstrates the usage of the LoRaWAN extension for LCTT (LoRaWAN Certification Test Tool) certification. It provides a reference implementation for running certification tests on Silicon Labs hardware using the LoRaWAN stack.

## Features
- Implements LCTT certification test cases
- Example configuration and source files included
- Logging and debug support

## Getting Started

1. **Prerequisites**
- Silicon Labs EFR32xG28 BRD4400C, SX1262MB BRD8042A Adapter PCB and Semtech SX1261/2 Evaluation Board
- Simplicity Studio or compatible build environment (Make File)
- LoRaWAN Certification Test Tool (LCTT)
- LoRaWAN Gateway and Network Server.

4. **Set Up LoRaWAN Gateway and Network Server**
- In this example, we used the Chirpstack Network Server. See the [Chirpstack Documentation v4](https://www.chirpstack.io) for more information.
- The LoRaWAN Gateway used in this example was the Dragino LPS8N LoRaWAN Gateway. See the [LPS8N -- LoRaWAN Gateway User Manual](https://wiki.dragino.com/xwiki/bin/view/Main/User%20Manual%20for%20All%20Gateway%20models/LPS8N%20-%20LoRaWAN%20Gateway%20User%20Manual) for more information.

5. **LoRaWAN User Credentials**

The LoRaWAN user credentials (DevEUI, JoinEUI, AppKey) must be configured before running the certification example.

Credentials are typically set in `example_options.h`

Update the following definitions with your device and network server values:

```c
#define USER_LORAWAN_DEVICE_EUI    { 0x00, 0x11, 0x22, ... } // 8 bytes
#define USER_LORAWAN_JOIN_EUI      { 0xAA, 0xBB, 0xCC, ... } // 8 bytes
#define USER_LORAWAN_GEN_APP_KEY   { 0xFF, 0xEE, 0xDD, ... } // 16 bytes
#define USER_LORAWAN_APP_KEY       { 0xFF, 0xEE, 0xDD, ... } // 16 bytes Network Key
```

Refer to your network server documentation for the correct values. Save your changes and rebuild the project before flashing.

## Start Example
1. **Join Network**
- Waiting for the log print following line, and press BTN0 on the WSTK to start example.
```
[D] INFO: Certification example is starting
[D] INFO: Push button to enable/disable certification
```
- Device will start LCTT Certification test mode, and automatically joined network. Waiting until we have following line.
- At Default, Uplink period is 5 seconds.
```
[D] INFO: Button pushed
...
[D] INFO: Event received: JOINED
[D] INFO: Modem is now joined
```
2. **Send MAC Command Manually**
- In Chirsptack Server Web Interface, access to `Application/<your_application_name>/<your_device_name>/Queue`
- In `Data`, enter you MAC Command and hit `EnQueue` to send Command to End Device.
- You can see all supported MAC Command at
3. **Send MAC Command using API**
- If you DO NOT want to send command manually, Chirpstack server support API for you to send command through Python scripts.
- Ensure you have Python in your environment.
- Install chirpstack API:
```
pip install chirpstack-api
```
- Access to `API` panel, Add your own API and get the TOKEN.
- Paste token to following scripts:
```Python
import os
import sys

import grpc
from chirpstack_api import api

# Configuration.

# This must point to the API interface.
server = "<your_network_server_IP>:8080"

# The DevEUI for which you want to enqueue the downlink.
dev_eui = "0101010101010101"

# The API token (retrieved using the web-interface).
api_token = "..."

if __name__ == "__main__":
  # Connect without using TLS.
  channel = grpc.insecure_channel(server)

  # Device-queue API client.
  client = api.DeviceServiceStub(channel)

  # Define the API key meta-data.
  auth_token = [("authorization", "Bearer %s" % api_token)]

  # Construct request.
  req = api.EnqueueDeviceQueueItemRequest()
  req.queue_item.confirmed = False
  req.queue_item.data = bytes([0xXX ...]) # MAC command and Payload (if needed)
  req.queue_item.dev_eui = dev_eui
  req.queue_item.f_port = 224

  resp = client.Enqueue(req, metadata=auth_token)

  # Print the downlink id
  print(resp.id)
```
