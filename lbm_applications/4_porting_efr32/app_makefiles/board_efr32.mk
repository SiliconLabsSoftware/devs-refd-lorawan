##############################################################################
# Definitions for the EFR32xG28 / EFR32xG24 board with Simplicity SDK
##############################################################################


SDK_ROOT := mcu_drivers/simplicity_sdk_2026.6.1
HAL_EFR32_DIR  := smtc_hal_efr32
$(info ************  board_$(BOARD)+++++++++ ************)

# -----------------------------------------------------------------------------
# Package-prefix aliases for Simplicity SDK
# -----------------------------------------------------------------------------
SDK_PCORE   := $(SDK_ROOT)/platform_core
SDK_PCMN    := $(SDK_PCORE)/platform/common
SDK_PAPPS   := $(SDK_PCORE)/app
SDK_DEV     := $(SDK_PCORE)/platform
SDK_BOARDS  := $(SDK_ROOT)/boards/hardware
SDK_BTL     := $(SDK_ROOT)/bootloader/platform
SDK_SEC     := $(SDK_DEV)/security
SDK_MBEDS   := $(SDK_ROOT)/mbedtls
SDK_HW      := $(SDK_PCORE)/hardware
SDK_FREERT  := $(SDK_ROOT)/freertos
SDK_CMSIS   := $(SDK_ROOT)/cmsis
SDK_CMSISC  := $(SDK_PCMN)

#-----------------------------------------------------------------------------
# Compilation flags
#-----------------------------------------------------------------------------

#MCU compilation flags
MCU_FLAGS ?= \
 -mcpu=cortex-m33 \
 -mthumb \
 -mfpu=fpv5-sp-d16 \
 -mfloat-abi=hard \
 -mcmse \
 -fdata-sections \
 -ffunction-sections \
 -fomit-frame-pointer \
 -fno-lto \
 -fmessage-length=0 \


BOARD_C_DEFS =  \
  -DNUMBER_OF_STACKS=1 \
  -DDEBUG_EFM=1 \
  -DSL_STACK_SIZE=4096 \
  -DSL_CODE_COMPONENT_SYSTEM=system \
  -DSL_CODE_COMPONENT_CLOCK_MANAGER=clock_manager \
  -DSL_COMPONENT_CATALOG_PRESENT=1 \
  -D_RTE_=1 \
  -DSL_CODE_COMPONENT_DEVICE_PERIPHERAL=device_peripheral \
  -DSL_CODE_COMPONENT_DMA_CHANNEL=dma_channel \
  -DSL_CODE_COMPONENT_DMA_MANAGER=dma_manager \
  -DSL_CODE_COMPONENT_GPIO=gpio \
  -DSL_CODE_COMPONENT_HAL_COMMON=hal_common \
  -DSL_CODE_COMPONENT_HAL_GPIO=hal_gpio \
  -DSL_CODE_COMPONENT_HAL_LDMA=hal_ldma \
  -DSL_CODE_COMPONENT_HAL_SYSRTC=hal_sysrtc \
  -DSL_CODE_COMPONENT_INTERRUPT_MANAGER=interrupt_manager \
  -DCMSIS_NVIC_VIRTUAL=1 \
  -DCMSIS_NVIC_VIRTUAL_HEADER_FILE=\"cmsis_nvic_virtual.h\" \
  -DMBEDTLS_CONFIG_FILE=\"sl_mbedtls_config.h\" \
  -DSL_CODE_COMPONENT_POWER_MANAGER=power_manager \
  -DMBEDTLS_PSA_CRYPTO_CONFIG_FILE=\"psa_crypto_config.h\" \
  -DSL_CODE_COMPONENT_SE_MANAGER=se_manager \
  -DSL_CODE_COMPONENT_CORE=core \
  -DSL_CODE_COMPONENT_SLEEPTIMER=sleeptimer \
  -DSL_CODE_COMPONENT_PSEC_OSAL=psec_osal \
  -DSL_CODE_COMPONENT_SPIDRV=spidrv \

ifeq ($(RTOS),yes)
BOARD_C_DEFS +=  \
  -DconfigNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS=2 \
  -DSL_CODE_COMPONENT_FREERTOS_KERNEL=freertos_kernel \

endif



ifeq ($(ALLOW_FUOTA),yes)
BOARD_LDSCRIPT =  $(SDK_ROOT)/../board/$(BOARD)/autogen/linkerfile_fuota.ld
else
BOARD_LDSCRIPT =  $(SDK_ROOT)/../board/$(BOARD)/autogen/linkerfile.ld
endif

OUTPUT_DIRECTORY := build_$(BOARD)


# $(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
#   LINKER_SCRIPT  := gcc_nrf52.ld

#-----------------------------------------------------------------------------
# Hardware-specific sources
#-----------------------------------------------------------------------------


BOARD_C_SOURCES = \
  $(SDK_DEV)/common/src/sl_core_cortexm.c \
  $(SDK_DEV)/driver/dma_channel/src/sl_dma_channel.c \
  $(SDK_DEV)/driver/dma_channel/src/sl_dma_descriptor_allocator.c \
  $(SDK_DEV)/driver/gpio/src/sl_gpio.c \
  $(SDK_DEV)/driver/leddrv/src/sl_led.c \
  $(SDK_DEV)/driver/leddrv/src/sl_simple_led.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_cache.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_default_common_linker.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_hal_flash.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_lock.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_object.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_page.c \
  $(SDK_DEV)/emdrv/nvm3/src/nvm3_utils.c \
  $(SDK_DEV)/emdrv/spidrv/src/spidrv.c \
  $(SDK_DEV)/emlib/src/em_burtc.c \
  $(SDK_DEV)/emlib/src/em_cmu.c \
  $(SDK_DEV)/emlib/src/em_emu.c \
  $(SDK_DEV)/emlib/src/em_eusart.c \
  $(SDK_DEV)/emlib/src/em_gpio.c \
  $(SDK_DEV)/emlib/src/em_iadc.c \
  $(SDK_DEV)/emlib/src/em_msc.c \
  $(SDK_DEV)/emlib/src/em_prs.c \
  $(SDK_DEV)/emlib/src/em_rmu.c \
  $(SDK_DEV)/emlib/src/em_system.c \
  $(SDK_DEV)/emlib/src/em_timer.c \
  $(SDK_DEV)/emlib/src/em_usart.c \
  $(SDK_DEV)/peripheral/src/sl_hal_eusart.c \
  $(SDK_DEV)/peripheral/src/sl_hal_gpio.c \
  $(SDK_DEV)/peripheral/src/sl_hal_ldma.c \
  $(SDK_DEV)/peripheral/src/sl_hal_letimer.c \
  $(SDK_DEV)/peripheral/src/sl_hal_prs.c \
  $(SDK_DEV)/peripheral/src/sl_hal_syscfg.c \
  $(SDK_DEV)/peripheral/src/sl_hal_sysrtc.c \
  $(SDK_DEV)/peripheral/src/sl_hal_sysrtc_subsystem.c \
  $(SDK_DEV)/peripheral/src/sl_hal_system.c \
  $(SDK_DEV)/peripheral/src/sl_hal_wdog.c \
  $(SDK_DEV)/service/clock_manager/src/sl_clock_manager.c \
  $(SDK_DEV)/service/clock_manager/src/sl_clock_manager_hal_s2.c \
  $(SDK_DEV)/service/clock_manager/src/sl_clock_manager_init.c \
  $(SDK_DEV)/service/clock_manager/src/sl_clock_manager_init_hal_s2.c \
  $(SDK_DEV)/service/device_init/src/sl_device_init_dcdc_s2.c \
  $(SDK_DEV)/service/device_manager/dma/sl_device_dma_s2.c \
  $(SDK_DEV)/service/device_manager/src/sl_device_clock.c \
  $(SDK_DEV)/service/device_manager/src/sl_device_dma.c \
  $(SDK_DEV)/service/device_manager/src/sl_device_gpio.c \
  $(SDK_DEV)/service/device_manager/src/sl_device_peripheral.c \
  $(SDK_DEV)/service/dma_manager/src/sl_dma_manager.c \
  $(SDK_DEV)/service/dma_manager/src/sl_dma_manager_hal_ldma.c \
  $(SDK_DEV)/service/hfxo_manager/src/sl_hfxo_manager.c \
  $(SDK_DEV)/service/hfxo_manager/src/sl_hfxo_manager_hal_s2.c \
  $(SDK_DEV)/service/interrupt_manager/src/sl_interrupt_manager_cortexm.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_dmadrv.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_retarget_stdio.c \
  $(SDK_DEV)/service/iostream/src/sl_iostream_stdlib_config.c \
  $(SDK_DEV)/service/memory_manager/src/sl_memory_manager.c \
  $(SDK_DEV)/service/memory_manager/src/sl_memory_manager_dynamic_reservation.c \
  $(SDK_DEV)/service/memory_manager/src/sl_memory_manager_pool.c \
  $(SDK_DEV)/service/memory_manager/src/sl_memory_manager_pool_common.c \
  $(SDK_DEV)/service/memory_manager/src/sl_memory_manager_region.c \
  $(SDK_DEV)/service/memory_manager/src/sl_memory_manager_retarget.c \
  $(SDK_DEV)/service/memory_manager/src/sli_memory_manager_common.c \
  $(SDK_DEV)/service/power_manager/src/common/sl_power_manager_common.c \
  $(SDK_DEV)/service/power_manager/src/common/sl_power_manager_em4.c \
  $(SDK_DEV)/service/power_manager/src/sleep_loop/sl_power_manager.c \
  $(SDK_DEV)/service/power_manager/src/sleep_loop/sl_power_manager_debug.c \
  $(SDK_DEV)/service/power_manager/src/sleep_loop/sl_power_manager_hal_s2.c \
  $(SDK_DEV)/service/sl_main/src/sl_main_init.c \
  $(SDK_DEV)/service/sl_main/src/sl_main_init_memory.c \
  $(SDK_DEV)/service/sleeptimer/src/sl_sleeptimer.c \
  $(SDK_DEV)/service/sleeptimer/src/sl_sleeptimer_hal_burtc.c \
  $(SDK_DEV)/service/sleeptimer/src/sl_sleeptimer_hal_sysrtc.c \
  $(SDK_DEV)/service/sleeptimer/src/sl_sleeptimer_hal_timer.c \
  $(SDK_DEV)/service/udelay/src/sl_udelay.c \
  $(SDK_PCMN)/src/sl_assert.c \
  $(SDK_PCMN)/src/sl_slist.c \
  $(SDK_PCMN)/src/sl_string.c \
  $(SDK_PCMN)/src/sl_syscalls.c \
  $(SDK_PAPPS)/common/util/app_log/app_log.c \
  $(SDK_BOARDS)/board/src/sl_board_control_gpio.c \
  $(SDK_BOARDS)/board/src/sl_board_init.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_attestation.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_cipher.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_entropy.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_hash.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_key_derivation.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_key_handling.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_signature.c \
  $(SDK_SEC)/sl_component/se_manager/src/sl_se_manager_util.c \
  $(SDK_SEC)/sl_component/se_manager/src/sli_se_manager_mailbox.c \
  $(SDK_SEC)/sl_component/sl_mbedtls_support/src/sl_mbedtls.c \
  $(SDK_SEC)/sl_component/sl_mbedtls_support/src/sl_psa_crypto.c \
  $(SDK_SEC)/sl_component/sl_mbedtls_support/src/sli_psa_crypto.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_psa_driver_common.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_psa_driver_init.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_psa_trng.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_aead.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_cipher.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_key_management.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_mac.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_driver_signature.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.c \
  $(SDK_SEC)/sl_component/sl_psa_driver/src/sli_se_version_dependencies.c \
  $(SDK_MBEDS)/library/cipher.c \
  $(SDK_MBEDS)/library/cipher_wrap.c \
  $(SDK_MBEDS)/library/constant_time.c \
  $(SDK_MBEDS)/library/platform.c \
  $(SDK_MBEDS)/library/platform_util.c \
  $(SDK_MBEDS)/library/psa_crypto.c \
  $(SDK_MBEDS)/library/psa_crypto_aead.c \
  $(SDK_MBEDS)/library/psa_crypto_cipher.c \
  $(SDK_MBEDS)/library/psa_crypto_client.c \
  $(SDK_MBEDS)/library/psa_crypto_driver_wrappers_no_static.c \
  $(SDK_MBEDS)/library/psa_crypto_ecp.c \
  $(SDK_MBEDS)/library/psa_crypto_ffdh.c \
  $(SDK_MBEDS)/library/psa_crypto_hash.c \
  $(SDK_MBEDS)/library/psa_crypto_mac.c \
  $(SDK_MBEDS)/library/psa_crypto_pake.c \
  $(SDK_MBEDS)/library/psa_crypto_rsa.c \
  $(SDK_MBEDS)/library/psa_crypto_se.c \
  $(SDK_MBEDS)/library/psa_crypto_slot_management.c \
  $(SDK_MBEDS)/library/psa_crypto_storage.c \
  $(SDK_MBEDS)/library/psa_util.c \
  $(SDK_MBEDS)/library/threading.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_board_default_init.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_dma_manager_instances.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_event_handler.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_iostream_handles.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_simple_led_instances.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_spidrv_init.c \
  $(HAL_EFR32_DIR)/smtc_hal_context.c \
  $(HAL_EFR32_DIR)/smtc_hal_crashlog.c \
  $(HAL_EFR32_DIR)/smtc_hal_flash.c \
  $(HAL_EFR32_DIR)/smtc_hal_gpio.c \
  $(HAL_EFR32_DIR)/smtc_hal_iadc.c \
  $(HAL_EFR32_DIR)/smtc_hal_lp_timer.c \
  $(HAL_EFR32_DIR)/smtc_hal_mcu.c \
  $(HAL_EFR32_DIR)/smtc_hal_rng.c \
  $(HAL_EFR32_DIR)/smtc_hal_rtc.c \
  $(HAL_EFR32_DIR)/smtc_hal_spi.c \
  $(HAL_EFR32_DIR)/smtc_hal_trace.c \
  $(HAL_EFR32_DIR)/smtc_hal_watchdog.c \
  smtc_modem_hal/smtc_modem_hal.c \

ifeq ($(RTOS),yes)
BOARD_C_SOURCES += \
  $(SDK_CMSIS)/RTOS2/Source/os_systick.c \
  $(SDK_CMSISC)/src/sl_cmsis_os2_common.c \
  $(SDK_CMSISC)/src/sli_cmsis_os2_ext_task_register.c \
  $(SDK_SEC)/sl_component/sli_psec_osal/src/sli_psec_osal_cmsis_rtos2.c \
  $(SDK_DEV)/service/sl_main/src/rtos/main_retarget.c \
  $(SDK_DEV)/service/sl_main/src/sl_main_kernel.c \
  $(SDK_FREERT)/cmsis/Source/cmsis_os2.c \
  $(SDK_FREERT)/kernel/croutine.c \
  $(SDK_FREERT)/kernel/event_groups.c \
  $(SDK_FREERT)/kernel/list.c \
  $(SDK_FREERT)/kernel/portable/MemMang/heap_3.c \
  $(SDK_FREERT)/kernel/portable/SiliconLabs/cortexm/tick_power_manager.c \
  $(SDK_FREERT)/kernel/queue.c \
  $(SDK_FREERT)/kernel/stream_buffer.c \
  $(SDK_FREERT)/kernel/tasks.c \
  $(SDK_FREERT)/kernel/timers.c \

endif

ifeq ($(ALLOW_FUOTA),yes)
BOARD_C_SOURCES += \
  $(SDK_BTL)/bootloader/api/btl_interface.c \
  $(SDK_BTL)/bootloader/api/btl_interface_storage.c \
  $(SDK_BTL)/bootloader/app_properties/app_properties.c \
  $(SDK_BTL)/bootloader/core/flash/btl_internal_flash.c \
  $(SDK_BTL)/bootloader/core/smp_switch/btl_smp_switch_flash_api.c \
  $(SDK_BTL)/bootloader/storage/internal_flash/btl_storage_internal_flash_raw.c \
  $(HAL_EFR32_DIR)/smtc_hal_bootloader.c \

endif


BOARD_ASM_SOURCES =  \
	$(SDK_DEV)/service/udelay/src/sl_udelay_armv6m_gcc.S \

# Manually add the udelay assembly object file
# EXTRA_OBJECTS += $(BUILD_DIR)/sl_udelay_armv6m_gcc.o

# Include folders common to all targets
BOARD_C_INCLUDES = \
  -I$(SDK_DEV)/common/errno_error_codes/inc \
  -I$(SDK_DEV)/common/inc \
  -I$(SDK_DEV)/driver/dma_channel/inc \
  -I$(SDK_DEV)/driver/gpio/inc \
  -I$(SDK_DEV)/driver/leddrv/inc \
  -I$(SDK_DEV)/emdrv/common/inc \
  -I$(SDK_DEV)/emdrv/nvm3/config \
  -I$(SDK_DEV)/emdrv/nvm3/inc \
  -I$(SDK_DEV)/emdrv/spidrv/inc \
  -I$(SDK_DEV)/emlib/inc \
  -I$(SDK_DEV)/peripheral/inc \
  -I$(SDK_DEV)/service/clock_manager/inc \
  -I$(SDK_DEV)/service/clock_manager/src \
  -I$(SDK_DEV)/service/device_init/inc \
  -I$(SDK_DEV)/service/device_manager/inc \
  -I$(SDK_DEV)/service/dma_manager/inc \
  -I$(SDK_DEV)/service/dma_manager/src \
  -I$(SDK_DEV)/service/hfxo_manager/inc \
  -I$(SDK_DEV)/service/interrupt_manager/inc \
  -I$(SDK_DEV)/service/interrupt_manager/inc/arm \
  -I$(SDK_DEV)/service/interrupt_manager/src \
  -I$(SDK_DEV)/service/iostream/inc \
  -I$(SDK_DEV)/service/memory_manager/inc \
  -I$(SDK_DEV)/service/memory_manager/src \
  -I$(SDK_DEV)/service/power_manager/inc \
  -I$(SDK_DEV)/service/power_manager/src/common \
  -I$(SDK_DEV)/service/power_manager/src/sleep_loop \
  -I$(SDK_DEV)/service/sl_main/inc \
  -I$(SDK_DEV)/service/sl_main/src \
  -I$(SDK_DEV)/service/sleeptimer/inc \
  -I$(SDK_DEV)/service/sleeptimer/src \
  -I$(SDK_DEV)/service/udelay/inc \
  -I$(SDK_PCMN)/inc \
  -I$(SDK_PAPPS)/common/util/app_log \
  -I$(SDK_BOARDS)/board/inc \
  -I$(SDK_SEC)/sl_component/se_manager/inc \
  -I$(SDK_SEC)/sl_component/sl_mbedtls_support/config \
  -I$(SDK_SEC)/sl_component/sl_mbedtls_support/config/preset \
  -I$(SDK_SEC)/sl_component/sl_mbedtls_support/inc \
  -I$(SDK_SEC)/sl_component/sl_psa_driver/inc \
  -I$(SDK_SEC)/sl_component/sli_psec_common/inc \
  -I$(SDK_SEC)/sl_component/sli_psec_osal/inc \
  -I$(SDK_MBEDS)/include \
  -I$(SDK_MBEDS)/library \
  -I$(SDK_CMSIS)/Core/Include \
  -I$(SDK_CMSISC)/inc \
  -I$(SDK_ROOT)/../board/$(BOARD)/autogen \
  -I$(SDK_ROOT)/../board/$(BOARD)/config \
  -I$(HAL_EFR32_DIR) \

ifeq ($(ALLOW_FUOTA),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_BTL)/bootloader \
  -I$(SDK_BTL)/bootloader/api \
  -I$(SDK_BTL)/bootloader/common \
  -I$(SDK_BTL)/bootloader/common/api \
  -I$(SDK_BTL)/bootloader/core \
  -I$(SDK_BTL)/bootloader/core/flash \
  -I$(SDK_BTL)/bootloader/core/smp_switch \

endif

ifeq ($(RTOS),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_CMSIS)/RTOS2/Include \
  -I$(SDK_DEV)/common/inc \
  -I$(SDK_PCMN)/inc \
  -I$(SDK_CMSISC)/inc \
  -I$(SDK_SEC)/sl_component/sl_mbedtls_support/inc \
  -I$(SDK_SEC)/sl_component/sli_psec_common/inc \
  -I$(SDK_SEC)/sl_component/sli_psec_osal/inc \
  -I$(SDK_DEV)/service/sl_main/inc \
  -I$(SDK_DEV)/service/sl_main/src \
  -I$(SDK_FREERT)/cmsis/Include \
  -I$(SDK_FREERT)/kernel/include \

BOARD_LDSCRIPT += -Wl,--wrap=main
endif

# Libraries common to all targets
LIB_FILES += \


ifeq ($(CRYPTO_HW_OVERRIDE),EFR32)
BOARD_C_SOURCES += smtc_modem_crypto/efr32_secure_element/smtc_secure_element_efr32.c
BOARD_C_INCLUDES += -Ismtc_modem_crypto/efr32_secure_element
BOARD_LDSCRIPT += @smtc_modem_crypto/efr32_secure_element/linker.options
endif


# .PHONY: default help

# Default target - first one defined
# default: brd4400c

# Print all targets that can be built
# help:
# 	@echo following targets are available:
# 	@echo		brd4400c
# 	@echo		sdk_config - starting external tool for editing sdk_config.h
# 	@echo		flash      - flashing binary

# TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


# include $(TEMPLATE_PATH)/Makefile.common

# $(foreach target, $(TARGETS), $(call define_target, $(target)))

# .PHONY: flash erase

# # Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/app_$(BOARD).hex
	# Add your EFR32 flash programming command here
	# commander flash $(OUTPUT_DIRECTORY)/app_$(BOARD).hex

# erase:
# 	# Add your EFR32 erase command here
# 	# commander device masserase

# SDK_CONFIG_FILE := ../config/sdk_config.h
# CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
# sdk_config:
# 	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
# 	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
