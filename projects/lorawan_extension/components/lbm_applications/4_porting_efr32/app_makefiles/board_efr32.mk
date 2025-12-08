##############################################################################
# Definitions for the EFR32ZG28 board with Simplicity SDK
##############################################################################


SDK_ROOT := mcu_drivers/simplicity_sdk_2025.6.0
HAL_EFR32_DIR  := smtc_hal_efr32
$(info ************  board_$(BOARD)+++++++++ ************)

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
  -DSL_CODE_COMPONENT_SYSTEM=system \
  -DSL_CODE_COMPONENT_CLOCK_MANAGER=clock_manager \
  -DSL_COMPONENT_CATALOG_PRESENT=1 \
  -DSL_CODE_COMPONENT_DEVICE_PERIPHERAL=device_peripheral \
  -DSL_CODE_COMPONENT_DMADRV=dmadrv \
  -DSL_CODE_COMPONENT_GPIO=gpio \
  -DSL_CODE_COMPONENT_HAL_COMMON=hal_common \
  -DSL_CODE_COMPONENT_HAL_GPIO=hal_gpio \
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
  $(SDK_ROOT)/app/common/util/app_log/app_log.c \
  $(SDK_ROOT)/hardware/board/src/sl_board_control_gpio.c \
  $(SDK_ROOT)/hardware/board/src/sl_board_init.c \
  $(SDK_ROOT)/hardware/driver/configuration_over_swo/src/sl_cos.c \
  $(SDK_ROOT)/hardware/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_eusart/sl_mx25_flash_shutdown.c \
  $(SDK_ROOT)/platform/common/src/sl_assert.c \
  $(SDK_ROOT)/platform/common/src/sl_core_cortexm.c \
  $(SDK_ROOT)/platform/common/src/sl_slist.c \
  $(SDK_ROOT)/platform/common/src/sl_string.c \
  $(SDK_ROOT)/platform/common/src/sl_syscalls.c \
  $(SDK_ROOT)/platform/driver/debug/src/sl_debug_swo.c \
  $(SDK_ROOT)/platform/driver/gpio/src/sl_gpio.c \
  $(SDK_ROOT)/platform/driver/leddrv/src/sl_led.c \
  $(SDK_ROOT)/platform/driver/leddrv/src/sl_simple_led.c \
  $(SDK_ROOT)/platform/emdrv/dmadrv/src/dmadrv.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_cache.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_default_common_linker.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_hal_flash.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_lock.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_object.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_page.c \
  $(SDK_ROOT)/platform/emdrv/nvm3/src/nvm3_utils.c \
  $(SDK_ROOT)/platform/emdrv/spidrv/src/spidrv.c \
  $(SDK_ROOT)/platform/emlib/src/em_burtc.c \
  $(SDK_ROOT)/platform/emlib/src/em_cmu.c \
  $(SDK_ROOT)/platform/emlib/src/em_emu.c \
  $(SDK_ROOT)/platform/emlib/src/em_rmu.c \
  $(SDK_ROOT)/platform/emlib/src/em_eusart.c \
  $(SDK_ROOT)/platform/emlib/src/em_gpio.c \
  $(SDK_ROOT)/platform/emlib/src/em_iadc.c \
  $(SDK_ROOT)/platform/emlib/src/em_ldma.c \
  $(SDK_ROOT)/platform/emlib/src/em_msc.c \
  $(SDK_ROOT)/platform/emlib/src/em_prs.c \
  $(SDK_ROOT)/platform/emlib/src/em_system.c \
  $(SDK_ROOT)/platform/emlib/src/em_timer.c \
  $(SDK_ROOT)/platform/emlib/src/em_usart.c \
  $(SDK_ROOT)/platform/peripheral/src/sl_hal_eusart.c \
  $(SDK_ROOT)/platform/peripheral/src/sl_hal_gpio.c \
  $(SDK_ROOT)/platform/peripheral/src/sl_hal_letimer.c \
  $(SDK_ROOT)/platform/peripheral/src/sl_hal_prs.c \
  $(SDK_ROOT)/platform/peripheral/src/sl_hal_sysrtc.c \
  $(SDK_ROOT)/platform/peripheral/src/sl_hal_wdog.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_attestation.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_cipher.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_entropy.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_hash.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_key_derivation.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_key_handling.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_signature.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sl_se_manager_util.c \
  $(SDK_ROOT)/platform/security/sl_component/se_manager/src/sli_se_manager_mailbox.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/src/sl_mbedtls.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/src/sl_psa_crypto.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/src/sli_psa_crypto.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_common.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_init.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_psa_trng.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_aead.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_cipher.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_management.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_mac.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_signature.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.c \
  $(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/src/sli_se_version_dependencies.c \
  $(SDK_ROOT)/platform/service/clock_manager/src/sl_clock_manager.c \
  $(SDK_ROOT)/platform/service/clock_manager/src/sl_clock_manager_hal_s2.c \
  $(SDK_ROOT)/platform/service/clock_manager/src/sl_clock_manager_init.c \
  $(SDK_ROOT)/platform/service/clock_manager/src/sl_clock_manager_init_hal_s2.c \
  $(SDK_ROOT)/platform/service/device_init/src/sl_device_init_dcdc_s2.c \
  $(SDK_ROOT)/platform/service/device_manager/src/sl_device_clock.c \
  $(SDK_ROOT)/platform/service/device_manager/src/sl_device_gpio.c \
  $(SDK_ROOT)/platform/service/device_manager/src/sl_device_peripheral.c \
  $(SDK_ROOT)/platform/service/hfxo_manager/src/sl_hfxo_manager.c \
  $(SDK_ROOT)/platform/service/hfxo_manager/src/sl_hfxo_manager_hal_s2.c \
  $(SDK_ROOT)/platform/service/interrupt_manager/src/sl_interrupt_manager_cortexm.c \
  $(SDK_ROOT)/platform/service/iostream/src/sl_iostream.c \
  $(SDK_ROOT)/platform/service/iostream/src/sl_iostream_eusart.c \
  $(SDK_ROOT)/platform/service/iostream/src/sl_iostream_retarget_stdio.c \
  $(SDK_ROOT)/platform/service/iostream/src/sl_iostream_stdlib_config.c \
  $(SDK_ROOT)/platform/service/iostream/src/sl_iostream_uart.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sl_memory_manager.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sl_memory_manager_dynamic_reservation.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sl_memory_manager_pool.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sl_memory_manager_pool_common.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sl_memory_manager_region.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sl_memory_manager_retarget.c \
  $(SDK_ROOT)/platform/service/memory_manager/src/sli_memory_manager_common.c \
  $(SDK_ROOT)/platform/service/power_manager/src/common/sl_power_manager_common.c \
  $(SDK_ROOT)/platform/service/power_manager/src/common/sl_power_manager_em4.c \
  $(SDK_ROOT)/platform/service/power_manager/src/sleep_loop/sl_power_manager.c \
  $(SDK_ROOT)/platform/service/power_manager/src/sleep_loop/sl_power_manager_debug.c \
  $(SDK_ROOT)/platform/service/power_manager/src/sleep_loop/sl_power_manager_hal_s2.c \
  $(SDK_ROOT)/platform/service/sl_main/src/sl_main_init.c \
  $(SDK_ROOT)/platform/service/sl_main/src/sl_main_init_memory.c \
  $(SDK_ROOT)/platform/service/sl_main/src/sl_main_process_action.c \
  $(SDK_ROOT)/platform/service/sleeptimer/src/sl_sleeptimer.c \
  $(SDK_ROOT)/platform/service/sleeptimer/src/sl_sleeptimer_hal_burtc.c \
  $(SDK_ROOT)/platform/service/sleeptimer/src/sl_sleeptimer_hal_sysrtc.c \
  $(SDK_ROOT)/platform/service/sleeptimer/src/sl_sleeptimer_hal_timer.c \
  $(SDK_ROOT)/platform/service/udelay/src/sl_udelay.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/cipher.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/cipher_wrap.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/constant_time.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/platform.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/platform_util.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_aead.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_cipher.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_client.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_driver_wrappers_no_static.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_ecp.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_ffdh.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_hash.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_mac.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_pake.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_rsa.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_se.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_slot_management.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_crypto_storage.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/psa_util.c \
  $(SDK_ROOT)/util/third_party/mbedtls/library/threading.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_board_default_init.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_event_handler.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_iostream_handles.c \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_iostream_init_eusart_instances.c \
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
  $(SDK_ROOT)/platform/CMSIS/RTOS2/Source/os_systick.c \
  $(SDK_ROOT)/platform/common/src/sl_cmsis_os2_common.c \
  $(SDK_ROOT)/platform/common/src/sli_cmsis_os2_ext_task_register.c \
  $(SDK_ROOT)/platform/security/sl_component/sli_psec_osal/src/sli_psec_osal_cmsis_rtos2.c \
  $(SDK_ROOT)/platform/service/sl_main/src/rtos/main_retarget.c \
  $(SDK_ROOT)/platform/service/sl_main/src/sl_main_kernel.c \
  $(SDK_ROOT)/util/third_party/freertos/cmsis/Source/cmsis_os2.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/croutine.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/event_groups.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/list.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/portable/MemMang/heap_3.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/portable/SiliconLabs/cortexm/tick_power_manager.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/queue.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/stream_buffer.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/tasks.c \
  $(SDK_ROOT)/util/third_party/freertos/kernel/timers.c \

else
BOARD_C_SOURCES += \
  $(SDK_ROOT)/../board/$(BOARD)/autogen/sl_power_manager_handler.c \

endif

ifeq ($(ALLOW_FUOTA),yes)
BOARD_C_SOURCES += \
  $(SDK_ROOT)/platform/bootloader/api/btl_interface.c \
  $(SDK_ROOT)/platform/bootloader/api/btl_interface_storage.c \
  $(SDK_ROOT)/platform/bootloader/app_properties/app_properties.c \
  $(SDK_ROOT)/platform/bootloader/core/flash/btl_internal_flash.c \
  $(HAL_EFR32_DIR)/smtc_hal_bootloader.c \

endif


BOARD_ASM_SOURCES =  \
	$(SDK_ROOT)/platform/service/udelay/src/sl_udelay_armv6m_gcc.S \

# Manually add the udelay assembly object file
# EXTRA_OBJECTS += $(BUILD_DIR)/sl_udelay_armv6m_gcc.o

# Include folders common to all targets
BOARD_C_INCLUDES = \
  -I$(SDK_ROOT)/app/common/util/app_log \
  -I$(SDK_ROOT)/platform/common/inc \
  -I$(SDK_ROOT)/hardware/board/inc \
  -I$(SDK_ROOT)/platform/service/clock_manager/inc \
  -I$(SDK_ROOT)/platform/service/clock_manager/src \
  -I$(SDK_ROOT)/platform/CMSIS/Core/Include \
  -I$(SDK_ROOT)/hardware/driver/configuration_over_swo/inc \
  -I$(SDK_ROOT)/platform/driver/debug/inc \
  -I$(SDK_ROOT)/platform/service/device_manager/inc \
  -I$(SDK_ROOT)/platform/service/device_init/inc \
  -I$(SDK_ROOT)/platform/emdrv/dmadrv/inc \
  -I$(SDK_ROOT)/platform/emdrv/dmadrv/inc/s2_signals \
  -I$(SDK_ROOT)/platform/emdrv/common/inc \
  -I$(SDK_ROOT)/platform/emlib/inc \
  -I$(SDK_ROOT)/platform/driver/gpio/inc \
  -I$(SDK_ROOT)/platform/peripheral/inc \
  -I$(SDK_ROOT)/platform/service/hfxo_manager/inc \
  -I$(SDK_ROOT)/platform/service/interrupt_manager/inc \
  -I$(SDK_ROOT)/platform/service/interrupt_manager/src \
  -I$(SDK_ROOT)/platform/service/interrupt_manager/inc/arm \
  -I$(SDK_ROOT)/platform/service/iostream/inc \
  -I$(SDK_ROOT)/platform/driver/leddrv/inc \
  -I$(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/config \
  -I$(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/config/preset \
  -I$(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/inc \
  -I$(SDK_ROOT)/util/third_party/mbedtls/include \
  -I$(SDK_ROOT)/util/third_party/mbedtls/library \
  -I$(SDK_ROOT)/platform/service/memory_manager/inc \
  -I$(SDK_ROOT)/platform/service/memory_manager/src \
  -I$(SDK_ROOT)/hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_eusart \
  -I$(SDK_ROOT)/platform/emdrv/nvm3/inc \
  -I$(SDK_ROOT)/platform/emdrv/nvm3/config \
  -I$(SDK_ROOT)/platform/service/power_manager/inc \
  -I$(SDK_ROOT)/platform/service/power_manager/src/common \
  -I$(SDK_ROOT)/platform/security/sl_component/sl_psa_driver/inc \
  -I$(SDK_ROOT)/platform/security/sl_component/se_manager/inc \
  -I$(SDK_ROOT)/platform/service/sl_main/inc \
  -I$(SDK_ROOT)/platform/service/sl_main/src \
  -I$(SDK_ROOT)/platform/service/sleeptimer/inc \
  -I$(SDK_ROOT)/platform/service/sleeptimer/src \
  -I$(SDK_ROOT)/platform/security/sl_component/sli_psec_osal/inc \
  -I$(SDK_ROOT)/platform/emdrv/spidrv/inc \
  -I$(SDK_ROOT)/platform/service/udelay/inc \
  -I$(SDK_ROOT)/platform/security/sl_component/sl_protocol_crypto/inc \
  -I$(SDK_ROOT)/util/silicon_labs/silabs_core/memory_manager/inc \
  -I$(SDK_ROOT)/../board/$(BOARD)/autogen \
  -I$(SDK_ROOT)/../board/$(BOARD)/config \
  -I$(HAL_EFR32_DIR) \

ifeq ($(RTOS),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_ROOT)/platform/CMSIS/RTOS2/Include \
  -I$(SDK_ROOT)/platform/common/inc \
  -I$(SDK_ROOT)/platform/security/sl_component/sl_mbedtls_support/inc \
  -I$(SDK_ROOT)/platform/security/sl_component/sli_psec_osal/inc \
  -I$(SDK_ROOT)/platform/service/sl_main/inc \
  -I$(SDK_ROOT)/platform/service/sl_main/src \
  -I$(SDK_ROOT)/util/third_party/freertos/cmsis/Include \
  -I$(SDK_ROOT)/util/third_party/freertos/kernel/include \

BOARD_LDSCRIPT += -Wl,--wrap=main
endif

ifeq ($(ALLOW_FUOTA),yes)
BOARD_C_INCLUDES += \
  -I$(SDK_ROOT)/platform/bootloader/api \
  -I$(SDK_ROOT)/platform/bootloader/core \
  -I$(SDK_ROOT)/platform/bootloader/core/flash \

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
