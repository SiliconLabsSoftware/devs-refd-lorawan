##############################################################################
# Definitions for the BRD4187C board (EFR32MG24) with Simplicity SDK
##############################################################################

BOARD_C_DEFS +=  \
  -DEFR32MG24B220F1536IM48=1 \
  -DSL_BOARD_NAME="BRD4187C" \
  -DSL_BOARD_REV="A02" \
  -DHARDWARE_BOARD_DEFAULT_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_3_RF_BANDS=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_915=1 \
  -DHFXO_FREQ=39000000 \

ASFLAGS += \
  -DEFR32MG24B220F1536IM48=1 \

BOARD_C_SOURCES += \
  $(SDK_DEV)/driver/debug/src/sl_debug_swo.c \
  $(SDK_DEV)/service/device_manager/clocks/sl_device_clock_efr32xg24.c \
  $(SDK_DEV)/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_swo.c \
  $(SDK_DEV)/Device/SiliconLabs/EFR32MG24/Source/startup_efr32mg24.c \
  $(SDK_DEV)/Device/SiliconLabs/EFR32MG24/Source/system_efr32mg24.c \
  $(SDK_BOARDS)/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_eusart/sl_mx25_flash_shutdown.c \
  $(SDK_HW)/driver/configuration_over_swo/src/sl_cos.c \

ifeq ($(RTOS),yes)
BOARD_C_SOURCES += \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/mpu_wrappers_v2_asm.c \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c \

endif

BOARD_C_INCLUDES += \
  -I$(SDK_DEV)/driver/debug/inc \
  -I$(SDK_DEV)/Device/SiliconLabs/EFR32MG24/Include \
  -I$(SDK_BOARDS)/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_eusart \
  -I$(SDK_HW)/driver/configuration_over_swo/inc \

ifeq ($(RTOS),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure \

endif
