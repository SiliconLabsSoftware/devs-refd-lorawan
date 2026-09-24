/*!
 * \file      hal_spi.h
 *
 * \brief     SPI Hardware Abstraction Layer wrapper for Silicon Labs EFR32xG28
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SMTC_HAL_SPI_H
#define SMTC_HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include "spidrv.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// SPI interface IDs compatible with Semtech HAL
#define RADIO_SPI_ID    0   // EUSART1 instance for radio communication

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*!
 * Sends out_data and receives in_data (byte-by-byte compatibility)
 * Used in: hal_spi_in_out(RADIO_SPI_ID, command[i]), hal_spi_in_out(RADIO_SPI_ID, 0)
 *
 * \param [IN] id       SPI interface id [0:N]
 * \param [IN] out_data Byte to be sent
 * \return     uint16_t Received byte
 */
uint16_t hal_spi_in_out_byte(const uint32_t id, const uint16_t out_data);

/*!
 * Sends out_data
 *
 * \param [IN] id       SPI interface id [0:N]
 * \param [IN] out_data Data array to be sent
 * \param [IN] data_len Number of bytes to be sent
 * \return     uint16_t Bytes sent
 */
uint16_t hal_spi_out(const uint32_t id, const uint8_t *out_data, const uint16_t data_len);

/*!
 * Receives data
 * \param [IN] id       SPI interface id [0:N]
 * \param [IN] in_data  Data array to be received
 * \param [IN] data_len Number of bytes to be received
 * \return     uint16_t Bytes received
 */
uint16_t hal_spi_in(const uint32_t id, uint8_t *in_data, const uint16_t data_len);

/*!
 * Set SPI handle for radio communication
 *
 * \param [IN] handle   SPIDRV handle for radio SPI
 */
void hal_spi_set_radio_handle(SPIDRV_Handle_t handle);

/*!
 * Initialize SPI HAL
 */
void hal_spi_init(void);

#ifdef __cplusplus
}
#endif

#endif  // SMTC_HAL_SPI_H

/* --- EOF ------------------------------------------------------------------ */
