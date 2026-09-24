##############################################################################
# Definitions for the Seeed XIAO MG24 board (EFR32MG24) with Simplicity SDK
##############################################################################

BOARD_C_DEFS +=  \
  -DEFR32MG24B220F1536IM48=1 \
  -DSL_BOARD_NAME="XIAO_MG24" \
  -DBOARD_XIAO_MG24=1 \
  -DSL_BOARD_REV="A01" \
  -DHARDWARE_BOARD_DEFAULT_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_3_RF_BANDS=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_868=1 \
  -DHARDWARE_BOARD_SUPPORTS_RF_BAND_915=1 \
  -DHFXO_FREQ=39000000 \

ASFLAGS += \
  -DEFR32MG24B220F1536IM48=1 \

BOARD_C_SOURCES += \
  $(SDK_DEV)/service/device_manager/clocks/sl_device_clock_efr32xg24.c \
  $(SDK_DEV)/service/device_manager/devices/sl_device_peripheral_hal_efr32xg24.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_eusart.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_uart.c \
  $(SDK_DEV)/Device/SiliconLabs/EFR32MG24/Source/startup_efr32mg24.c \
  $(SDK_DEV)/Device/SiliconLabs/EFR32MG24/Source/system_efr32mg24.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_iostream_init_eusart_instances.c \

ifeq ($(RTOS),yes)
BOARD_C_SOURCES += \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/mpu_wrappers_v2_asm.c \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
  $(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c \

endif

BOARD_C_INCLUDES += \
  -I$(SDK_DEV)/Device/SiliconLabs/EFR32MG24/Include \

ifeq ($(RTOS),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_FREERT)/kernel/portable/GCC/ARM_CM33_NTZ/non_secure \

endif

#-----------------------------------------------------------------------------
# Ping-pong fixed role (Wio-SX1262 has a single user button)
#-----------------------------------------------------------------------------
PING_PONG_ROLE ?=

ifeq ($(MODEM_APP),PING_PONG)
ifeq ($(PING_PONG_ROLE),PING)
BOARD_C_DEFS += -DPING_PONG_ROLE_PING=1
else ifeq ($(PING_PONG_ROLE),PONG)
BOARD_C_DEFS += -DPING_PONG_ROLE_PONG=1
else
$(error xiao_mg24 + PING_PONG requires PING_PONG_ROLE=PING or PING_PONG_ROLE=PONG)
endif
endif
