//
// Created by jack on 7/19/20.
//

#include "sys_task.h"
#include "nrf52.h"

// app includes
#include "app_settings.h"
#include "app_battery.h"
#include "app_sensors.h"

// component includes
#include "bma421.h"
#include "common.h"
#include "HRS3300.h"
#include "battery.h"
#include "button.h"

// TODO: temporary
#include "CST816S.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#include "display.h"
#include "haptic.h"

// RTOS Variables
extern SemaphoreHandle_t button_semphr;
extern TaskHandle_t thDisplay;
extern TaskHandle_t thSysTask;
extern EventGroupHandle_t component_event_group;
extern EventGroupHandle_t charging_event_group;
extern EventGroupHandle_t error_event_group;


// System State Variables
System_t sys = {
    .initialized = false,
    .sleep = false,
    .wakeup = false,
};


static bool init_system_startup(void)
{
    EventBits_t set_bits = xEventGroupGetBits(error_event_group);

//    if(!bma_init())
//    {
//        bma423_get_device_id();
//        set_bits |= COMPONENT_SENSOR_IMU;
//    }
    if(HRS3300_init())
    {
        set_bits |= COMPONENT_SENSOR_HRS;
        NRF_LOG_INFO("HRS3300 initialized");
    }
    else
    {
        NRF_LOG_INFO("HRS3300 not init'd!");
    }

    if(CST816S_init())
    {
        NRF_LOG_INFO("CST816S initialized");
    }
    else
    {
        NRF_LOG_INFO("CST816S not init'd!");
    }

    haptic_init();
    update_battery_state();
    return true;
}


/** Public Functions **/
void sys_task(void * arg)
{
    NRF_LOG_INFO("Init SystemTask");
    UNUSED_PARAMETER(arg);

    uint32_t touchscreen_pressed = false;
    init_system_startup();
    sys.initialized = true;
    NRF_LOG_INFO("SysTask Init'd");

    uint16_t x, y;

    while(1)
    {
        run_battery_app();
//        run_accel_app();
        run_heart_rate_app();
//        run_settings_app();
        run_haptic_app();

        if(xTaskNotifyWait(0, 0, &touchscreen_pressed, 0))
        {
            CST816S_read_touch(&x, &y);
        }
//        run_sensor_update_display();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void system_button_handler(void)
{
    // Resume tasks if suspended
    if(eTaskGetState(thSysTask) == eSuspended)
    {
        vTaskResume(thSysTask);
    }

    if(eTaskGetState(thDisplay) == eSuspended)
    {
        vTaskResume(thDisplay);
    }

    // Notify display task
    xTaskNotify(thDisplay, 1, eNoAction);
}

