/***************************************************************************//**
 * @file smtc_config.h
 * @brief The LBM Configuration Header File
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SMTC_CONFIG_H
#define SMTC_CONFIG_H

#include "sl_component_catalog.h"
#if defined(SL_CATALOG_LBM_RADIO_EUSART_PRESENT)
#include "sl_spidrv_eusart_radio_config.h"
#elif defined(SL_CATALOG_LBM_RADIO_USART_PRESENT)
#include "sl_spidrv_usart_radio_config.h"
#endif

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Sample application Name

// <o SAMPLE_APP_NAME> Duty Cycle ON time in microseconds
// <i> Default: "My Sample App Name"
// <i> String representation of the sample application name
#define SAMPLE_APP_NAME      "My Sample App Name"

// </h> Sample application Name

// <h> TCXO (Temperature Compensated Crystal Oscillator) enablement

// <q XOSC_CFG_TCXO_ENABLE> Enable TCXO support
// <i> Default: enabled
#ifndef XOSC_CFG_TCXO_ENABLE
#define XOSC_CFG_TCXO_ENABLE            1
#endif
// </h>

// <h> Flash config
// <q USE_FLASH_READ_MODIFY_WRITE> Enable Flash Read-Modify-Write
// <i> Default: disabled
#ifndef USE_FLASH_READ_MODIFY_WRITE
#define USE_FLASH_READ_MODIFY_WRITE     1
#endif
// </h>

// RF Switch config
#ifndef LBM_ANT_SWITCH_MANUAL
#define LBM_ANT_SWITCH_MANUAL   0
#endif


// <<< end of configuration section >>>

#if defined(SL_CATALOG_LBM_RADIO_EUSART_PRESENT)
#define RADIO_SPI_MOSI_PORT     SL_SPIDRV_EUSART_RADIO_TX_PORT
#define RADIO_SPI_MOSI_PIN      SL_SPIDRV_EUSART_RADIO_TX_PIN
#define RADIO_SPI_MISO_PORT     SL_SPIDRV_EUSART_RADIO_RX_PORT
#define RADIO_SPI_MISO_PIN      SL_SPIDRV_EUSART_RADIO_RX_PIN
#define RADIO_SPI_SCLK_PORT     SL_SPIDRV_EUSART_RADIO_SCLK_PORT
#define RADIO_SPI_SCLK_PIN      SL_SPIDRV_EUSART_RADIO_SCLK_PIN
#define RADIO_NSS_PORT          SL_SPIDRV_EUSART_RADIO_CS_PORT
#define RADIO_NSS_PIN           SL_SPIDRV_EUSART_RADIO_CS_PIN
#define RADIO_BUSY_PIN          SMTC_RADIO_CTL_BUSY_PIN
#define RADIO_BUSY_PORT         SMTC_RADIO_CTL_BUSY_PORT
#define RADIO_RESET_PIN         SMTC_RADIO_CTL_RESET_PIN
#define RADIO_RESET_PORT        SMTC_RADIO_CTL_RESET_PORT
#define RADIO_DIOX_PORT         SMTC_RADIO_CTL_DIOX_PORT
#define RADIO_DIOX_PIN          SMTC_RADIO_CTL_DIOX_PIN
#define RADIO_ANTENNA_SWITCH_PORT    SMTC_RADIO_CTL_ANTSW_PORT
#define RADIO_ANTENNA_SWITCH_PIN     SMTC_RADIO_CTL_ANTSW_PIN

// Radio DIOX IRQ config
#define RADIO_DIOX_IRQ_MODE     SL_GPIO_INTERRUPT_RISING_EDGE
#define RADIO_DIOX_PULL_MODE    SMTC_HAL_GPIO_PULL_MODE_UP
#endif

#endif // SMTC_CONFIG_H
