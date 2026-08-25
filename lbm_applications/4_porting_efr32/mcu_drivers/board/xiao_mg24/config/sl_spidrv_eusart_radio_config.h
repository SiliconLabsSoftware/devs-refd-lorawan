/***************************************************************************//**
 * @file
 * @brief SPIDRV_EUSART Config — Seeed XIAO MG24 + Wio-SX1262 for XIAO
 *******************************************************************************
 * Seeed XIAO MG24 (EFR32MG24B220F1536IM48) with the Wio-SX1262 for XIAO
 * shield (Seeed SKU 113010003) stacked on the standard XIAO header.
 *
 * Radio SPI on EUSART1 (XIAO SPI0 pads, application-controlled NSS):
 *   SCK  PA03 (XIAO D8 / SCK0)
 *   MISO PA04 (XIAO D9 / MISO0)
 *   MOSI PA05 (XIAO D10 / MOSI0)
 *   NSS  PC04 (XIAO D4 / SDA0 — manual CS, drives the shield's NSS)
 *
 * Radio control GPIOs from the Wio-SX1262 header:
 *   DIO1 PC01 (XIAO D1)
 *   RST  PC02 (XIAO D2)
 *   BUSY PC03 (XIAO D3)
 *   RXEN PC05 (XIAO D5) — antenna switch RX enable; the SX1262 internal
 *                         DIO2 already drives the TX side of the RF switch.
 *
 * NOTE: PA06 is the internal SPI flash CS, PA07 is the on-board USER_LED,
 *       PD03/PD04 are wired to the battery network — they MUST NOT be used
 *       for the radio on this board.
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SPIDRV_EUSART_RADIO_CONFIG_H
#define SL_SPIDRV_EUSART_RADIO_CONFIG_H

#include "spidrv.h"

// <<< Use Configuration Wizard in Context Menu >>>
// <h> SPIDRV settings

// <o SL_SPIDRV_EUSART_RADIO_BITRATE> SPI bitrate
// <i> Default: 1000000
#define SL_SPIDRV_EUSART_RADIO_BITRATE           1000000

// <o SL_SPIDRV_EUSART_RADIO_FRAME_LENGTH> SPI frame length <7-16>
// <i> Default: 8
#define SL_SPIDRV_EUSART_RADIO_FRAME_LENGTH      8

// <o SL_SPIDRV_EUSART_RADIO_TYPE> SPI mode
// <spidrvMaster=> Master
// <spidrvSlave=> Slave
#define SL_SPIDRV_EUSART_RADIO_TYPE              spidrvMaster

// <o SL_SPIDRV_EUSART_RADIO_BIT_ORDER> Bit order on the SPI bus
// <spidrvBitOrderLsbFirst=> LSB transmitted first
// <spidrvBitOrderMsbFirst=> MSB transmitted first
#define SL_SPIDRV_EUSART_RADIO_BIT_ORDER         spidrvBitOrderMsbFirst

// <o SL_SPIDRV_EUSART_RADIO_CLOCK_MODE> SPI clock mode
// <spidrvClockMode0=> SPI mode 0: CLKPOL=0, CLKPHA=0
// <spidrvClockMode1=> SPI mode 1: CLKPOL=0, CLKPHA=1
// <spidrvClockMode2=> SPI mode 2: CLKPOL=1, CLKPHA=0
// <spidrvClockMode3=> SPI mode 3: CLKPOL=1, CLKPHA=1
#define SL_SPIDRV_EUSART_RADIO_CLOCK_MODE        spidrvClockMode0

// <o SL_SPIDRV_EUSART_RADIO_CS_CONTROL> SPI master chip select (CS) control scheme.
// <spidrvCsControlAuto=> CS controlled by the SPI driver
// <spidrvCsControlApplication=> CS controlled by the application
#define SL_SPIDRV_EUSART_RADIO_CS_CONTROL        spidrvCsControlApplication

// <o SL_SPIDRV_EUSART_RADIO_SLAVE_START_MODE> SPI slave transfer start scheme
// <spidrvSlaveStartImmediate=> Transfer starts immediately
// <spidrvSlaveStartDelayed=> Transfer starts when the bus is idle (CS deasserted)
// <i> Only applies if instance type is spidrvSlave
#define SL_SPIDRV_EUSART_RADIO_SLAVE_START_MODE  spidrvSlaveStartImmediate
// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,SCLK,(CS)> SL_SPIDRV_EUSART_RADIO
// $[EUSART_SL_SPIDRV_EUSART_RADIO]
#ifndef SL_SPIDRV_EUSART_RADIO_PERIPHERAL       
#define SL_SPIDRV_EUSART_RADIO_PERIPHERAL        EUSART1
#endif
#ifndef SL_SPIDRV_EUSART_RADIO_PERIPHERAL_NO    
#define SL_SPIDRV_EUSART_RADIO_PERIPHERAL_NO     1
#endif

// EUSART1 TX (MOSI) on PA05 — XIAO D10
#ifndef SL_SPIDRV_EUSART_RADIO_TX_PORT          
#define SL_SPIDRV_EUSART_RADIO_TX_PORT           SL_GPIO_PORT_A
#endif
#ifndef SL_SPIDRV_EUSART_RADIO_TX_PIN           
#define SL_SPIDRV_EUSART_RADIO_TX_PIN            5
#endif

// EUSART1 RX (MISO) on PA04 — XIAO D9
#ifndef SL_SPIDRV_EUSART_RADIO_RX_PORT          
#define SL_SPIDRV_EUSART_RADIO_RX_PORT           SL_GPIO_PORT_A
#endif
#ifndef SL_SPIDRV_EUSART_RADIO_RX_PIN           
#define SL_SPIDRV_EUSART_RADIO_RX_PIN            4
#endif

// EUSART1 SCLK on PA03 — XIAO D8
#ifndef SL_SPIDRV_EUSART_RADIO_SCLK_PORT        
#define SL_SPIDRV_EUSART_RADIO_SCLK_PORT         SL_GPIO_PORT_A
#endif
#ifndef SL_SPIDRV_EUSART_RADIO_SCLK_PIN         
#define SL_SPIDRV_EUSART_RADIO_SCLK_PIN          3
#endif

// EUSART1 CS on PC04 — XIAO D4 (manual / application-controlled)
#ifndef SL_SPIDRV_EUSART_RADIO_CS_PORT          
#define SL_SPIDRV_EUSART_RADIO_CS_PORT           SL_GPIO_PORT_C
#endif
#ifndef SL_SPIDRV_EUSART_RADIO_CS_PIN           
#define SL_SPIDRV_EUSART_RADIO_CS_PIN            4
#endif
// [EUSART_SL_SPIDRV_EUSART_RADIO]$

// <gpio signal=BUSY,RST> SMTC_RADIO_CTL
// $[GPIO_SMTC_RADIO_CTL]
#define SMTC_RADIO_CTL_PERIPHERAL           GPIO

// BUSY on PC03 — XIAO D3
#define SMTC_RADIO_CTL_BUSY_PORT           SL_GPIO_PORT_C
#define SMTC_RADIO_CTL_BUSY_PIN            3

// RST on PC02 — XIAO D2
#define SMTC_RADIO_CTL_RESET_PORT          SL_GPIO_PORT_C
#define SMTC_RADIO_CTL_RESET_PIN           2

// DIO1 (IRQ) on PC01 — XIAO D1
#define SMTC_RADIO_CTL_DIO1_PORT           SL_GPIO_PORT_C
#define SMTC_RADIO_CTL_DIO1_PIN            1

// RF switch (RXEN) on PC05 — XIAO D5/SCL
#define SMTC_RADIO_CTL_ANTSW_PORT          SL_GPIO_PORT_C
#define SMTC_RADIO_CTL_ANTSW_PIN           5
// [GPIO_SMTC_RADIO_CTL]$

// <<< sl:end pin_tool >>>

#endif // SL_SPIDRV_EUSART_RADIO_CONFIG_H

