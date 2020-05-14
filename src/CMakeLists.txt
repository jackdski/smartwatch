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
nRF5_addBLEANCS()
nRF5_addBLE_DB_Discovery()
nRF5_addBLE_GQ()
nRF5_addBLE_GATTS()
nRF5_addBLEService("ble_cts_c")

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

set(LVGL_SRC
        lv_conf.h
        lvgl/lvgl.h
        lvgl/src/lv_core/lv_obj.c
        lvgl/src/lv_core/lv_obj.h
        lvgl/src/lv_core/lv_group.c
        lvgl/src/lv_core/lv_group.h
        lvgl/src/lv_core/lv_disp.c
        lvgl/src/lv_core/lv_disp.h
        lvgl/src/lv_core/lv_debug.h
        lvgl/src/lv_core/lv_debug.c
        lvgl/src/lv_core/lv_indev.c
        lvgl/src/lv_core/lv_indev.h
        lvgl/src/lv_core/lv_refr.c
        lvgl/src/lv_core/lv_refr.h
        lvgl/src/lv_core/lv_style.c
        lvgl/src/lv_core/lv_style.h

        lvgl/src/lv_misc/lv_anim.c
        lvgl/src/lv_misc/lv_anim.h
        lvgl/src/lv_misc/lv_async.h
        lvgl/src/lv_misc/lv_async.c
        lvgl/src/lv_misc/lv_fs.c
        lvgl/src/lv_misc/lv_fs.h
        lvgl/src/lv_misc/lv_task.c
        lvgl/src/lv_misc/lv_task.h
        lvgl/src/lv_misc/lv_area.c
        lvgl/src/lv_misc/lv_area.h
        lvgl/src/lv_misc/lv_bidi.c
        lvgl/src/lv_misc/lv_bidi.h
        lvgl/src/lv_misc/lv_circ.c
        lvgl/src/lv_misc/lv_circ.h
        lvgl/src/lv_misc/lv_color.c
        lvgl/src/lv_misc/lv_color.h
        lvgl/src/lv_misc/lv_fs.c
        lvgl/src/lv_misc/lv_fs.h
        lvgl/src/lv_misc/lv_gc.c
        lvgl/src/lv_misc/lv_gc.h
        lvgl/src/lv_misc/lv_ll.c
        lvgl/src/lv_misc/lv_ll.h
        lvgl/src/lv_misc/lv_log.c
        lvgl/src/lv_misc/lv_log.h
        lvgl/src/lv_misc/lv_math.c
        lvgl/src/lv_misc/lv_math.h
        lvgl/src/lv_misc/lv_mem.c
        lvgl/src/lv_misc/lv_mem.h
        lvgl/src/lv_misc/lv_printf.c
        lvgl/src/lv_misc/lv_printf.h
        lvgl/src/lv_misc/lv_task.c
        lvgl/src/lv_misc/lv_task.h
        lvgl/src/lv_misc/lv_templ.c
        lvgl/src/lv_misc/lv_templ.h
        lvgl/src/lv_misc/lv_txt.c
        lvgl/src/lv_misc/lv_txt.h
        lvgl/src/lv_misc/lv_types.h
        lvgl/src/lv_misc/lv_utils.c
        lvgl/src/lv_misc/lv_utils.h

        lvgl/src/lv_draw/lv_draw.c
        lvgl/src/lv_draw/lv_draw.h
        lvgl/src/lv_draw/lv_draw_arc.c
        lvgl/src/lv_draw/lv_draw_arc.h
        lvgl/src/lv_draw/lv_draw_basic.c
        lvgl/src/lv_draw/lv_draw_basic.h
        lvgl/src/lv_draw/lv_draw_img.c
        lvgl/src/lv_draw/lv_draw_img.h
        lvgl/src/lv_draw/lv_draw_label.c
        lvgl/src/lv_draw/lv_draw_label.h
        lvgl/src/lv_draw/lv_draw_line.c
        lvgl/src/lv_draw/lv_draw_line.h
        lvgl/src/lv_draw/lv_draw_rect.c
        lvgl/src/lv_draw/lv_draw_rect.h
        lvgl/src/lv_draw/lv_draw_triangle.c
        lvgl/src/lv_draw/lv_draw_triangle.h
        lvgl/src/lv_draw/lv_img_cache.c
        lvgl/src/lv_draw/lv_img_cache.h
        lvgl/src/lv_draw/lv_img_decoder.c
        lvgl/src/lv_draw/lv_img_decoder.h

        lvgl/src/lv_hal/lv_hal.h
        lvgl/src/lv_hal/lv_hal_disp.c
        lvgl/src/lv_hal/lv_hal_disp.h
        lvgl/src/lv_hal/lv_hal_indev.c
        lvgl/src/lv_hal/lv_hal_indev.h
        lvgl/src/lv_hal/lv_hal_tick.c
        lvgl/src/lv_hal/lv_hal_tick.h

        lvgl/src/lv_font/lv_font.c
        lvgl/src/lv_font/lv_font.h
        lvgl/src/lv_font/lv_font_fmt_txt.c
        lvgl/src/lv_font/lv_font_fmt_txt.h
        #  lvgl/src/lv_font/lv_font_roboto_16.c
        lvgl/src/lv_font/lv_symbol_def.h

        lvgl/src/lv_themes/lv_theme.c
        lvgl/src/lv_themes/lv_theme.h

        lvgl/src/lv_objx/lv_btn.h
        lvgl/src/lv_objx/lv_btn.c

        lvgl/src/lv_objx/lv_cont.h
        lvgl/src/lv_objx/lv_cont.c

        lvgl/src/lv_objx/lv_label.h
        lvgl/src/lv_objx/lv_label.c

        lvgl/src/lv_themes/lv_theme.c
        lvgl/src/lv_themes/lv_theme.h
        lvgl/src/lv_themes/lv_theme_night.h
        lvgl/src/lv_themes/lv_theme_night.c

        lvgl/src/lv_objx/lv_list.c
        lvgl/src/lv_objx/lv_list.h
        lvgl/src/lv_objx/lv_tileview.c
        lvgl/src/lv_objx/lv_tileview.h
        lvgl/src/lv_objx/lv_tabview.c
        lvgl/src/lv_objx/lv_tabview.h
        lvgl/src/lv_objx/lv_btnm.c
        lvgl/src/lv_objx/lv_btnm.h
        lvgl/src/lv_objx/lv_page.c
        lvgl/src/lv_objx/lv_page.h
        lvgl/src/lv_objx/lv_img.c
        lvgl/src/lv_objx/lv_img.h
        lvgl/src/lv_objx/lv_lmeter.c
        lvgl/src/lv_objx/lv_lmeter.h
        lvgl/src/lv_objx/lv_arc.c
        lvgl/src/lv_objx/lv_arc.h
        lvgl/src/lv_objx/lv_gauge.c
        lvgl/src/lv_objx/lv_gauge.h

        lvgl/src/lv_objx/lv_mbox.c
        lvgl/src/lv_objx/lv_mbox.h

        lvgl/src/lv_objx/lv_bar.c
        lvgl/src/lv_objx/lv_bar.h
        lvgl/src/lv_objx/lv_slider.h
        lvgl/src/lv_objx/lv_slider.c

        )

# include files
list(APPEND SOURCE_FILES
        main.c
        ble/ble_general.h
        ble/ble_general.c
        ble/ble_cus.h
        ble/ble_cus.c
        ble/ble_ancs.h
        ble/ble_ancs.c
        drivers/drv2605l.h
        drivers/drv2605l.c
        drivers/ssd1306.h
        drivers/ssd1306.c
        components/sensors.h
        components/sensors.c
        components/display.h
        components/display.c
        FreeRTOSConfig.h
        ${FREERTOS_SRC}
        ${LVGL_SRC}
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