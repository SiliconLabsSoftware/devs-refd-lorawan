# LCTT Certification Example

This example runs LoRaWAN Certification Test Tool (LCTT) scenarios on Silicon Labs EFR32 with LoRa Basics Modem and SX1262. Push the user button to enable or disable certification mode (FPort **224**).

## Features

- LCTT certification mode toggle via BTN0
- Periodic uplinks while joined (default **10 s**)
- Credentials and region in [example_options.h](example_options.h)
- Debug console logging

## Requirements

See [Hardware](../README.md#hardware) and [Software requirements](../README.md#software-requirements) in the EFR32 porting README.

Also needed for this example: LCTT (or equivalent test tooling), LoRaWAN gateway, and network server.

## Credentials

Edit [example_options.h](example_options.h):

```c
#define USER_LORAWAN_DEVICE_EUI    { /* 8 bytes */ }
#define USER_LORAWAN_JOIN_EUI      { /* 8 bytes */ }
#define USER_LORAWAN_GEN_APP_KEY   { /* 16 bytes */ }
#define USER_LORAWAN_APP_KEY       { /* 16 bytes */ }
#define MODEM_EXAMPLE_REGION       SMTC_MODEM_REGION_EU_868
```

Rebuild after changing credentials.

## Build

From `lbm_applications/4_porting_efr32`:

```bash
make sx1262 BOARD=brd4405a MODEM_APP=LCTT_CERTIF
```

Firmware: `build_sx1262_<board>/app_sx1262.hex`

## Gateway and network server

- ChirpStack: [documentation](https://www.chirpstack.io)
- Example gateway: Dragino LPS8N ([manual](https://wiki-old.dragino.com/xwiki/bin/view/Main/User%20Manual%20for%20All%20Gateway%20models/LPS8N%20-%20LoRaWAN%20Gateway%20User%20Manual))

Provision the device with the same DevEUI / JoinEUI / keys as in `example_options.h`.

## How to run

### 1. Join and enable certification

After reset you should see:

```
[D] INFO: Certification example is starting
[D] INFO: Push button to enable/disable certification
```

Press **BTN0** to enable certification mode. The device joins and then sends uplinks every **10 seconds**.

```
[D] INFO: Button pushed
...
[D] INFO: Event received: JOINED
[D] INFO: Modem is now joined
```

Press BTN0 again to disable certification mode.

### 2. Send MAC commands manually (ChirpStack UI)

1. Open `Application / <app> / <device> / Queue`
2. Enter the command bytes in **Data** and enqueue
3. Use **f_port = 224** for certification-protocol traffic when the device is in certification mode

Supported certification MAC commands are defined in **TS009 LoRaWAN Certification Protocol, section 5**  
([TS009-1.2.1](https://resources.lora-alliance.org/technical-specifications/ts009-1-2-1-certification-protocol)).

### 3. Send MAC commands via ChirpStack API (Python)

```bash
pip install chirpstack-api
```

Create an API token in the ChirpStack **API** panel, then:

```python
import grpc
from chirpstack_api import api

# Configuration
server = "<your_network_server_IP>:8080"
dev_eui = "0101010101010101"
api_token = "..."

if __name__ == "__main__":
    channel = grpc.insecure_channel(server)
    client = api.DeviceServiceStub(channel)
    auth_token = [("authorization", "Bearer %s" % api_token)]

    req = api.EnqueueDeviceQueueItemRequest()
    req.queue_item.confirmed = False
    # Certification protocol commands must use f_port 224 (TS009).
    # Payload: package identifier / command bytes per TS009 section 5.
    req.queue_item.data = bytes([0x00])  # replace with real command bytes
    req.queue_item.dev_eui = dev_eui
    req.queue_item.f_port = 224

    resp = client.Enqueue(req, metadata=auth_token)
    print(resp.id)
```

**Notes on f_port 224:**

- Port 224 is reserved for the LoRaWAN certification protocol
- The device only accepts certification traffic when certification mode is enabled (BTN0)
- Do not use application ports for TS009 test commands
