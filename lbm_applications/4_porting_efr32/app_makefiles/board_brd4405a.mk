##############################################################################
# Definitions for the BRD4405A board with Simplicity SDK
##############################################################################

# BRD4405A uses EFR32FG28 (see main_porting_tests.cmake).
BOARD_C_DEFS +=  \
  -DEFR32FG28B320F1024IM48=1 \
  -DSL_BOARD_NAME="BRD4405A" \
  -DBOARD_BRD4405A=1 \
  -DSL_BOARD_REV="A02" \
  -DHARDWARE_BOARD_DEFAULT_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_3_RF_BANDS=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_915=1 \
  -DHFXO_FREQ=39000000 \

ASFLAGS += \
    -DEFR32FG28B320F1024IM48=1 \

BOARD_C_SOURCES += \
  $(SDK_DEV)/driver/debug/src/sl_debug_swo.c \
  $(SDK_DEV)/service/device_manager/clocks/sl_device_clock_efr32xg28.c \
  $(SDK_DEV)/service/device_manager/devices/sl_device_peripheral_hal_efr32xg28.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_eusart.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_uart.c \
  $(SDK_DEV)/Device/SiliconLabs/EFR32FG28/Source/startup_efr32fg28.c \
  $(SDK_DEV)/Device/SiliconLabs/EFR32FG28/Source/system_efr32fg28.c \
  $(SDK_HW)/driver/configuration_over_swo/src/sl_cos.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_iostream_init_eusart_instances.c \

ifeq ($(RTOS),yes)
BOARD_C_SOURCES += \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/mpu_wrappers_v2_asm.c \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c \

endif

BOARD_C_INCLUDES += \
  -I$(SDK_DEV)/driver/debug/inc \
  -I$(SDK_DEV)/Device/SiliconLabs/EFR32FG28/Include \
  -I$(SDK_HW)/driver/configuration_over_swo/inc \

ifeq ($(RTOS),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure \

endif
