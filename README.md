# LoRaWAN PoC for Series 2 devices

The project is a Proof of Concept (PoC) focusing on the stack porting and enable using some sample applications for evaluation. The purpose of this project is to port the LoRa Basic Modem to the Silicon Labs EFR32FG28 microcontroller platform using the Semtech SX1262/LLCC68 radio transceivers. This will enable low-power, long-range wireless communication for embedded applications.

## Features

- LoRaWAN stack implementation for EFR32 Series 2 devices
- Support for Semtech SX1262/LLCC68 radio transceivers
- Sample applications for evaluation and testing
- Integration with Silicon Labs Simplicity Studio 5
- Pre-built binaries for quick testing

## Getting Started

For detailed setup instructions, hardware requirements, software installation, and usage examples, please refer to the [User Guide](projects/lorawan_extension/docs/RefD_LoRaWAN_User_Guide_v1.0.0.md).


## Project Structure

```
├── projects/lorawan_extension/     # Main LoRaWAN extension
│   ├── components/                 # Hardware abstraction components
│   │   ├── lbm_applications/
│   │   │   └── 4_porting_efr32/
│   │   │       ├── main_example/  # Example applications
│   │   │       ├── test/          # Unit tests and test applications
│   ├── config/                    # Configuration files
│   ├── docs/                      # Documentation
│   ├── examples/                  # Example projects
│   ├── scripts/                   # Build and utility scripts
│   └── test/                      # Test suites
├── resources/
│   ├── binaries/                  # Pre-built firmware binaries
│   ├── documents/                 # Additional documentation
│   └── images/                    # Documentation images
└── README.md                      # This file
```
## Contributing
Please follow the [CONTRIBUTING](./.github/CONTRIBUTING.md) guidelines.

## License
See the [LICENSE.md](./LICENSE.md) file for details.
