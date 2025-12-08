##############################################################################
# Definitions for the BRD4400C board with Simplicity SDK
##############################################################################

BOARD_C_DEFS +=  \
  -DEFR32ZG28B312F1024IM68=1 \
  -DSL_BOARD_NAME="BRD4400C" \
  -DSL_BOARD_REV="A01" \
  -DHARDWARE_BOARD_DEFAULT_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_3_RF_BANDS=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_915=1 \
  -DHFXO_FREQ=39000000 \

ASFLAGS += \
    -DEFR32ZG28B312F1024IM68=1 \

BOARD_C_SOURCES += \
  $(SDK_ROOT)/platform/Device/SiliconLabs/EFR32ZG28/Source/startup_efr32zg28.c \
  $(SDK_ROOT)/platform/Device/SiliconLabs/EFR32ZG28/Source/system_efr32zg28.c \
  $(SDK_ROOT)/platform/service/device_manager/clocks/sl_device_clock_efr32xg28.c \
  $(SDK_ROOT)/platform/service/device_manager/devices/sl_device_peripheral_hal_efr32xg28.c \

ifeq ($(RTOS),yes)
BOARD_C_SOURCES += \
  $(SDK_ROOT)/util/third_party/freertos/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/mpu_wrappers_v2_asm.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c \

endif

BOARD_C_INCLUDES += \
  -I$(SDK_ROOT)/platform/Device/SiliconLabs/EFR32ZG28/Include \

ifeq ($(RTOS),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_ROOT)/util/third_party/freertos/kernel/portable/GCC/ARM_CM33_NTZ/non_secure \

endif
