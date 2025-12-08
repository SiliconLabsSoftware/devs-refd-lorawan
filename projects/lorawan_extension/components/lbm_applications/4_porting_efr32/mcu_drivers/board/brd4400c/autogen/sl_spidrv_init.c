#include "spidrv.h"
#include "sl_spidrv_instances.h"
#include "sl_assert.h"


#include "sl_spidrv_eusart_radio_config.h"
SPIDRV_HandleData_t sl_spidrv_eusart_radio_handle_data;
SPIDRV_Handle_t sl_spidrv_eusart_radio_handle = &sl_spidrv_eusart_radio_handle_data;

SPIDRV_Init_t sl_spidrv_eusart_init_radio = {
  .port = SL_SPIDRV_EUSART_RADIO_PERIPHERAL,
  .portTx = SL_SPIDRV_EUSART_RADIO_TX_PORT,
  .portRx = SL_SPIDRV_EUSART_RADIO_RX_PORT,
  .portClk = SL_SPIDRV_EUSART_RADIO_SCLK_PORT,
#if defined(SL_SPIDRV_EUSART_RADIO_CS_PORT)
  .portCs = SL_SPIDRV_EUSART_RADIO_CS_PORT,
#endif
  .pinTx = SL_SPIDRV_EUSART_RADIO_TX_PIN,
  .pinRx = SL_SPIDRV_EUSART_RADIO_RX_PIN,
  .pinClk = SL_SPIDRV_EUSART_RADIO_SCLK_PIN,
#if defined(SL_SPIDRV_EUSART_RADIO_CS_PIN)
  .pinCs = SL_SPIDRV_EUSART_RADIO_CS_PIN,
#endif
  .bitRate = SL_SPIDRV_EUSART_RADIO_BITRATE,
  .frameLength = SL_SPIDRV_EUSART_RADIO_FRAME_LENGTH,
  .dummyTxValue = 0,
  .type = SL_SPIDRV_EUSART_RADIO_TYPE,
  .bitOrder = SL_SPIDRV_EUSART_RADIO_BIT_ORDER,
  .clockMode = SL_SPIDRV_EUSART_RADIO_CLOCK_MODE,
  .csControl = SL_SPIDRV_EUSART_RADIO_CS_CONTROL,
  .slaveStartMode = SL_SPIDRV_EUSART_RADIO_SLAVE_START_MODE,
};

void sl_spidrv_init_instances(void) {
#if !defined(SL_SPIDRV_EUSART_RADIO_CS_PIN)
  EFM_ASSERT(sl_spidrv_eusart_init_radio.csControl == spidrvCsControlApplication);
#endif 
  SPIDRV_Init(sl_spidrv_eusart_radio_handle, &sl_spidrv_eusart_init_radio);
}
