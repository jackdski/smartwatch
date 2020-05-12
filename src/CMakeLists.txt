set(NRF5_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/gcc_${NRF_FAMILY})

# DFU requirements
# List the softdevice versions previously used, or use FALSE if no previous softdevices
set(PREVIOUS_SOFTDEVICES FALSE)
# Set the location to the DFU private key
set(PRIVATE_KEY ${CMAKE_CURRENT_SOURCE_DIR}/../keys/dfu_private.key)
set(PUBLIC_KEY ${CMAKE_CURRENT_SOURCE_DIR}/../keys/dfu_public_key.c)
# Set the App validation type. [NO_VALIDATION|VALIDATE_GENERATED_CRC|VALIDATE_GENERATED_SHA256|VALIDATE_ECDSA_P256_SHA256]
set(APP_VALIDATION_TYPE NO_VALIDATION)
# Set the Soft Device validation type. [NO_VALIDATION|VALIDATE_GENERATED_CRC|VALIDATE_GENERATED_SHA256|VALIDATE_ECDSA_P256_SHA256]
set(SD_VALIDATION_TYPE NO_VALIDATION)
# The bootloader version (user defined)
set(BOOTLOADER_VERSION 1)
# The DFU version string (firmware version string)
set(DFU_VERSION_STRING "${VERSION_STRING}")

# Set the target name
set(target jd_smartwatch)

# add the required libraries
nRF5_addLog()
nRF5_addSeggerRTT()
nRF5_addAppError()

nRF5_addAppFIFO()
nRF5_addLegacyPPI()
nRF5_addPPI()
nRF5_addTimer()
nRF5_addClock()
nRF5_addQueue()
nRF5_addAppTimer()
nRF5_addAppUART()
nRF5_addAppTWI()
nRF5_addAppButton()
nRF5_addAppPwm()
nRF5_addPowerMgmt()
nRF5_addBSP(TRUE FALSE FALSE)
nRF5_addBLEGATT()
nRF5_addBLEQWR()
nRF5_addBLEAdvertising()
nRF5_addBLEPeerManager()
nRF5_addBLELinkCtxManager()

include_directories(.)
include_directories(FreeRTOS/Source/portable/GCC/ARM_CM4F)
include_directories(FreeRTOS/Source/include)
include_directories(ble)

set(FREERTOS_SRC
        FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
        FreeRTOS/Source/portable/GCC/ARM_CM4F/port_cmsis.c
        FreeRTOS/Source/portable/GCC/ARM_CM4F/port_cmsis_systick.c
        FreeRTOS/Source/croutine.c
        FreeRTOS/Source/event_groups.c
        FreeRTOS/Source/list.c
        FreeRTOS/Source/queue.c
        FreeRTOS/Source/tasks.c
        FreeRTOS/Source/timers.c
        FreeRTOS/Source/include/timers.h
        FreeRTOS/Source/portable/MemMang/heap_4.c
        )

# include files
list(APPEND SOURCE_FILES
        main.c
        ble/ble_general.h
        ble/ble_general.c
        ble/ble_cus.h
        ble/ble_cus.c
        drivers/drv2605l.h
        drivers/drv2605l.c
        components/sensors.h
        components/sensors.c
        FreeRTOSConfig.h
        ${FREERTOS_SRC}
        )

list(APPEND INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

nRF5_addExecutable(${target} "${SOURCE_FILES}" "${INCLUDE_DIRS}" "${NRF5_LINKER_SCRIPT}")

# make sdk_config.h import app_config.h
target_compile_definitions(${target} PRIVATE USE_APP_CONFIG)

# Here you can set a list of user variables to be defined in the bootloader makefile (which you have modified yourself)
set(bootloader_vars "")

# add the secure bootloader build target
nRF5_addSecureBootloader(${target} "${PUBLIC_KEY}" "${bootloader_vars}")
# add the bootloader merge target
nRF5_addBootloaderMergeTarget(${target} ${DFU_VERSION_STRING} ${PRIVATE_KEY} ${PREVIOUS_SOFTDEVICES} ${APP_VALIDATION_TYPE} ${SD_VALIDATION_TYPE} ${BOOTLOADER_VERSION})
# add the bootloader merged flash target
nRF5_addFlashTarget(bl_merge_${target} "${CMAKE_CURRENT_BINARY_DIR}/${target}_bl_merged.hex")
# Add the Bootloader + SoftDevice + App package target
nRF5_addDFU_BL_SD_APP_PkgTarget(${target} ${DFU_VERSION_STRING} ${PRIVATE_KEY} ${PREVIOUS_SOFTDEVICES} ${APP_VALIDATION_TYPE} ${SD_VALIDATION_TYPE} ${BOOTLOADER_VERSION})
# Add the App package target
nRF5_addDFU_APP_PkgTarget(${target} ${DFU_VERSION_STRING} ${PRIVATE_KEY} ${PREVIOUS_SOFTDEVICES} ${APP_VALIDATION_TYPE})

# print the size of consumed RAM and flash - does not yet work on Windows
if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    nRF5_print_size(${target} ${NRF5_LINKER_SCRIPT} TRUE)
endif()