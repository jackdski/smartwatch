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
set(target example)

# add the required libraries
nRF5_addSystemNRF()
nRF5_addLog()
nRF5_addSeggerRTT()
nRF5_addAppError()

nRF5_addClock()
nRF5_addSoftDeviceSupport()

include_directories(FreeRTOS/Source/portable/GCC/ARM_CM4F)
include_directories(FreeRTOS/Source/portable/)
include_directories(FreeRTOS/Source/include)

include_directories(components)
include_directories(components/sensors/)
include_directories(components/sensors/BMA423-Sensor-API-master)
include_directories(components/ble)
include_directories(components/display/)
include_directories(components/display/brightness)
include_directories(components/display/heart_rate)
include_directories(components/display/settings)
include_directories(components/display/steps)

# Libraries
#nRF5_addFreeRTOSPort()
nRF5_addAtomic()
nRF5_addBalloc()
nRF5_addQueue()
nRF5_addPowerMgmt()
nRF5_addGPIOTE()
nRF5_addPRS()
nRF5_addSAADC()
nRF5_addSPI()
nRF5_addLegacySPI()
nRF5_addAppTWI()
nRF5_addLegacyPPI()
nRF5_addAppPwm()

# BLE
nRF5_addAppTimer()
nRF5_addTimer()
nRF5_addMutex()
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

        lvgl/src/lv_core/lv_disp.c
        lvgl/src/lv_core/lv_disp.h
        lvgl/src/lv_core/lv_group.c
        lvgl/src/lv_core/lv_group.h
        lvgl/src/lv_core/lv_indev.c
        lvgl/src/lv_core/lv_indev.h
        lvgl/src/lv_core/lv_obj.c
        lvgl/src/lv_core/lv_obj.h
        lvgl/src/lv_core/lv_obj_style_dec.h
        lvgl/src/lv_core/lv_refr.c
        lvgl/src/lv_core/lv_refr.h
        lvgl/src/lv_core/lv_style.c
        lvgl/src/lv_core/lv_style.h

        lvgl/src/lv_draw/lv_draw.h
        lvgl/src/lv_draw/lv_draw_arc.c
        lvgl/src/lv_draw/lv_draw_arc.h
        lvgl/src/lv_draw/lv_draw_blend.c
        lvgl/src/lv_draw/lv_draw_blend.h
        lvgl/src/lv_draw/lv_draw_img.c
        lvgl/src/lv_draw/lv_draw_img.h
        lvgl/src/lv_draw/lv_draw_label.c
        lvgl/src/lv_draw/lv_draw_label.h
        lvgl/src/lv_draw/lv_draw_line.c
        lvgl/src/lv_draw/lv_draw_line.h
        lvgl/src/lv_draw/lv_draw_mask.c
        lvgl/src/lv_draw/lv_draw_mask.h
        lvgl/src/lv_draw/lv_draw_rect.c
        lvgl/src/lv_draw/lv_draw_rect.h
        lvgl/src/lv_draw/lv_draw_triangle.c
        lvgl/src/lv_draw/lv_draw_triangle.h
        lvgl/src/lv_draw/lv_img_buf.c
        lvgl/src/lv_draw/lv_img_buf.h
        lvgl/src/lv_draw/lv_img_cache.c
        lvgl/src/lv_draw/lv_img_cache.h
        lvgl/src/lv_draw/lv_img_decoder.c
        lvgl/src/lv_draw/lv_img_decoder.h

        lvgl/src/lv_font/lv_font.c
        lvgl/src/lv_font/lv_font.h
        lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.c
        lvgl/src/lv_font/lv_font_fmt_txt.c
        lvgl/src/lv_font/lv_font_fmt_txt.h
        lvgl/src/lv_font/lv_font_montserrat_12.c
        lvgl/src/lv_font/lv_font_montserrat_12_subpx.c
        lvgl/src/lv_font/lv_font_montserrat_14.c
        lvgl/src/lv_font/lv_font_montserrat_16.c
        lvgl/src/lv_font/lv_font_montserrat_18.c
        lvgl/src/lv_font/lv_font_montserrat_20.c
        lvgl/src/lv_font/lv_font_montserrat_22.c
        lvgl/src/lv_font/lv_font_montserrat_24.c
        lvgl/src/lv_font/lv_font_montserrat_26.c
        lvgl/src/lv_font/lv_font_montserrat_28.c
        lvgl/src/lv_font/lv_font_montserrat_28_compressed.c
        lvgl/src/lv_font/lv_font_montserrat_30.c
        lvgl/src/lv_font/lv_font_montserrat_32.c
        lvgl/src/lv_font/lv_font_montserrat_34.c
        lvgl/src/lv_font/lv_font_montserrat_36.c
        lvgl/src/lv_font/lv_font_montserrat_38.c
        lvgl/src/lv_font/lv_font_montserrat_40.c
        lvgl/src/lv_font/lv_font_montserrat_42.c
        lvgl/src/lv_font/lv_font_montserrat_44.c
        lvgl/src/lv_font/lv_font_montserrat_46.c
        lvgl/src/lv_font/lv_font_montserrat_48.c
        lvgl/src/lv_font/lv_font_simsun_16_cjk.c
        lvgl/src/lv_font/lv_font_unscii_8.c
        lvgl/src/lv_font/lv_symbol_def.h

        lvgl/src/lv_hal/lv_hal.h
        lvgl/src/lv_hal/lv_hal_disp.c
        lvgl/src/lv_hal/lv_hal_disp.h
        lvgl/src/lv_hal/lv_hal_indev.c
        lvgl/src/lv_hal/lv_hal_indev.h
        lvgl/src/lv_hal/lv_hal_tick.c
        lvgl/src/lv_hal/lv_hal_tick.h

        lvgl/src/lv_misc/lv_anim.c
        lvgl/src/lv_misc/lv_anim.h
        lvgl/src/lv_misc/lv_area.c
        lvgl/src/lv_misc/lv_area.h
        lvgl/src/lv_misc/lv_async.h
        lvgl/src/lv_misc/lv_async.c
        lvgl/src/lv_misc/lv_bidi.c
        lvgl/src/lv_misc/lv_bidi.h
        lvgl/src/lv_misc/lv_color.c
        lvgl/src/lv_misc/lv_color.h
        lvgl/src/lv_misc/lv_debug.c
        lvgl/src/lv_misc/lv_debug.h
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
        lvgl/src/lv_misc/lv_txt_ap.c
        lvgl/src/lv_misc/lv_txt_ap.h
        lvgl/src/lv_misc/lv_types.h
        lvgl/src/lv_misc/lv_utils.c
        lvgl/src/lv_misc/lv_utils.h

        lvgl/src/lv_themes/lv_theme.c
        lvgl/src/lv_themes/lv_theme.h
#        lvgl/src/lv_themes/lv_theme_empty.c
#        lvgl/src/lv_themes/lv_theme_empty.h
#        lvgl/src/lv_themes/lv_theme_material.c
#        lvgl/src/lv_themes/lv_theme_material.h
        lvgl/src/lv_themes/lv_theme_mono.c
        lvgl/src/lv_themes/lv_theme_mono.h
#        lvgl/src/lv_themes/lv_theme_template.c
#        lvgl/src/lv_themes/lv_theme_template.h

        lvgl/src/lv_widgets/lv_arc.c
        lvgl/src/lv_widgets/lv_arc.h
        lvgl/src/lv_widgets/lv_bar.c
        lvgl/src/lv_widgets/lv_bar.h
        lvgl/src/lv_widgets/lv_btn.c
        lvgl/src/lv_widgets/lv_btn.h
        lvgl/src/lv_widgets/lv_btnmatrix.c
        lvgl/src/lv_widgets/lv_btnmatrix.h
        lvgl/src/lv_widgets/lv_calendar.c
        lvgl/src/lv_widgets/lv_calendar.h
        lvgl/src/lv_widgets/lv_canvas.c
        lvgl/src/lv_widgets/lv_canvas.h
        lvgl/src/lv_widgets/lv_chart.c
        lvgl/src/lv_widgets/lv_chart.h
        lvgl/src/lv_widgets/lv_checkbox.c
        lvgl/src/lv_widgets/lv_checkbox.h
        lvgl/src/lv_widgets/lv_cont.c
        lvgl/src/lv_widgets/lv_cont.h
        lvgl/src/lv_widgets/lv_cpicker.c
        lvgl/src/lv_widgets/lv_cpicker.h
        lvgl/src/lv_widgets/lv_dropdown.c
        lvgl/src/lv_widgets/lv_dropdown.h
        lvgl/src/lv_widgets/lv_gauge.c
        lvgl/src/lv_widgets/lv_gauge.h
        lvgl/src/lv_widgets/lv_img.c
        lvgl/src/lv_widgets/lv_img.h
        lvgl/src/lv_widgets/lv_imgbtn.c
        lvgl/src/lv_widgets/lv_imgbtn.h
        lvgl/src/lv_widgets/lv_keyboard.c
        lvgl/src/lv_widgets/lv_keyboard.h
        lvgl/src/lv_widgets/lv_label.c
        lvgl/src/lv_widgets/lv_label.h
        lvgl/src/lv_widgets/lv_led.c
        lvgl/src/lv_widgets/lv_led.h
        lvgl/src/lv_widgets/lv_line.c
        lvgl/src/lv_widgets/lv_line.h
        lvgl/src/lv_widgets/lv_linemeter.c
        lvgl/src/lv_widgets/lv_linemeter.h
        lvgl/src/lv_widgets/lv_list.c
        lvgl/src/lv_widgets/lv_list.h
        lvgl/src/lv_widgets/lv_msgbox.c
        lvgl/src/lv_widgets/lv_msgbox.h
        lvgl/src/lv_widgets/lv_objmask.c
        lvgl/src/lv_widgets/lv_objmask.h
        lvgl/src/lv_widgets/lv_objx_templ.c
        lvgl/src/lv_widgets/lv_objx_templ.h
        lvgl/src/lv_widgets/lv_page.c
        lvgl/src/lv_widgets/lv_page.h
        lvgl/src/lv_widgets/lv_roller.c
        lvgl/src/lv_widgets/lv_roller.h
        lvgl/src/lv_widgets/lv_slider.c
        lvgl/src/lv_widgets/lv_slider.h
        lvgl/src/lv_widgets/lv_spinbox.c
        lvgl/src/lv_widgets/lv_spinbox.h
        lvgl/src/lv_widgets/lv_spinner.c
        lvgl/src/lv_widgets/lv_spinner.h
        lvgl/src/lv_widgets/lv_switch.c
        lvgl/src/lv_widgets/lv_switch.h
        lvgl/src/lv_widgets/lv_table.c
        lvgl/src/lv_widgets/lv_table.h
        lvgl/src/lv_widgets/lv_tabview.c
        lvgl/src/lv_widgets/lv_tabview.h
        lvgl/src/lv_widgets/lv_textarea.c
        lvgl/src/lv_widgets/lv_textarea.h
        lvgl/src/lv_widgets/lv_tileview.c
        lvgl/src/lv_widgets/lv_tileview.h
        lvgl/src/lv_widgets/lv_win.c
        lvgl/src/lv_widgets/lv_win.h
        )

# include files
list(APPEND SOURCE_FILES
        main.c
        components/ble/ble_general.h
        components/ble/ble_general.c
#        ble/ble_update.h
#        ble/ble_update.c
        components/ble/ble_ancs.h
        components/ble/ble_ancs.c
        drivers/SGM40561.h
        drivers/SGM40561.c
        drivers/twi_driver.h
        drivers/twi_driver.c
        drivers/spi_driver.h
        drivers/spi_driver.c
#        drivers/ST7735.h
#        drivers/ST7735.c
        drivers/ST7789.h
        drivers/ST7789.c
        drivers/CST816S.h
        drivers/CST816S.c
        drivers/display_drv.h
        drivers/display_drv.c
        drivers/peripherals.h
        drivers/peripherals.c
        components/common.h
        components/resources.h
        components/resources.c
        components/battery.h
        components/battery.c
        drivers/button.h
        drivers/button.c
        components/sys_task.h
        components/sys_task.c
        components/haptic.h
        components/haptic.c
        components/display/brightness/display_brightness.h
        components/display/brightness/display_brightness.c
        components/display/heart_rate/display_heart_rate.h
        components/display/heart_rate/display_heart_rate.c
        components/display/settings/display_settings.h
        components/display/settings/display_settings.c
        components/display/steps/display_steps.h
        components/display/steps/display_steps.c
        components/display/display_common.h
        components/display/display_common.c
        components/display/display_boot_up.h
        components/display/display_boot_up.c
        components/display/home.h
        components/display/home.c
        components/display/display.h
        components/display/display.c
        components/sensors
        components/sensors/BMA423-Sensor-API-master/bma4_defs.h
        components/sensors/BMA423-Sensor-API-master/bma4.h
        components/sensors/BMA423-Sensor-API-master/bma4.c
        components/sensors/BMA423-Sensor-API-master/bma423.h
        components/sensors/BMA423-Sensor-API-master/bma423.c
        components/sensors/bma421.h
        components/sensors/bma421.c
        components/sensors/HRS3300.h
        components/sensors/HRS3300.c
        "${SDK_ROOT}/components/softdevice/common/nrf_sdh_freertos.c"
        FreeRTOSConfig.h
        FreeRTOS.c
        ${FREERTOS_SRC}
        ${LVGL_SRC}
        )
list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

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