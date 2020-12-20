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
#include "HRS3300.h"
#include "battery.h"
#include "button.h"

#include "nrf_temp.h"

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
#include "resources.h"
#include "haptic.h"

// RTOS Variables
extern SemaphoreHandle_t twi_semphr;
extern SemaphoreHandle_t button_semphr;
extern TaskHandle_t thDisplay;
extern TaskHandle_t thSysTask;
extern EventGroupHandle_t component_event_group;
extern EventGroupHandle_t charging_event_group;

// System State Variables
System_t sys = {
    .initialized = false,
    .sleep = false,
    .wakeup = false,
};


/** Private Functions **/
//static void init_temp_measurement(void)
//{
//    nrf_temp_init();
//}
//
//static int32_t read_temp(void)
//{
//    NRF_TEMP->TASKS_START = 1;
//    while(NRF_TEMP->EVENTS_DATARDY == 0);
//    NRF_TEMP->EVENTS_DATARDY = 0;
//    volatile int32_t temp = (nrf_temp_read() / 4);
//    NRF_TEMP->TASKS_STOP = 1;
//    NRF_LOG_INFO("Temp: %d", temp);
//    return temp;
//}

static bool init_system_startup(void)
{
    bool bma_initialized = false, HRS3300_initialized = false;
    EventBits_t set_bits = xEventGroupGetBits(component_event_group);

    bma_initialized = bma_init();
    HRS3300_initialized = HRS3300_init();

    haptic_init();

//    set_bits |= COMPONENT_HAPTIC;

//    if(bma_initialized == true)
//    {
//        set_bits |= COMPONENT_SENSOR_IMU;
//    }
//    if(HRS3300_initialized == true)
//    {
//        set_bits |= COMPONENT_SENSOR_HRS;
//    }

//    update_battery_state();
//    set_bits |= COMPONENT_BATTERY_MONITOR;

    // set bits and wait up to 10s for remaining bits to be set before performing a system reset
//    bool ret = false;
//    xEventGroupSetBits(component_event_group, set_bits);
//    EventBits_t returned_bits;
//    returned_bits = xEventGroupWaitBits(component_event_group,
//                                        COMPONENT_LIST_ALL,
//                                        pdTRUE,
//                                        pdTRUE,
//                                        pdMS_TO_TICKS(10000));
//
//    if((returned_bits & COMPONENT_LIST_ALL) == COMPONENT_LIST_ALL)
//    {
//        ret = true;
//    }
//    return ret;
    return true;
}


/** Public Functions **/
void sys_task(void * arg)
{
    NRF_LOG_INFO("Init SystemTask");
    UNUSED_PARAMETER(arg);

    haptic_init();
//    init_temp_measurement();

    bma423_get_device_id();
    HRS3300_enable();
    HRS3300_get_device_id();

    bma_init();
    HRS3300_init();
    config_button(system_button_handler);

    sys.initialized = true;
    NRF_LOG_INFO("SysTask Init'd");
    volatile int32_t temperature = 0;

    while(1)
    {
//        run_battery_app();
//        run_accel_app();
//        run_heart_rate_app();
//        run_settings_app();
        run_haptic_app();
//        run_sensor_update_display();
//        read_temp();
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
    xSemaphoreGive(button_semphr);
}

