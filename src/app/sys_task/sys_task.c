//
// Created by jack on 7/19/20.
//

#include "sys_task.h"
#include "nrf52.h"

#include "features.h"

// app includes
#include "app_settings.h"
#include "app_sensors.h"

// component includes
#include "bma421.h"
#include "callbacks.h"
#include "common.h"
#include "CST816S.h"
#include "HRS3300.h"
#include "battery.h"
#include "side_button.h"

#include "display.h"
#include "haptic.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"


// RTOS Variables
extern EventGroupHandle_t error_event_group;


// System State Variables
System_t sys = {
    .initialized    = false,
    .sleep          = false,
    .wakeup         = false,
    .state          = SYSTEM_INITIALIZATION,
    .free_heap      = 0
};


static bool init_system_startup(void)
{
#if(FEATURE_BMA_IMU)
    if(bma_init())
    {
        bma423_get_device_id();
    }
#endif

    init_HRS3300();
    init_CST816S();
    init_haptic();
    init_gpio_interrupts();

    return true;
}


/** Public Functions **/
void sys_task(void * arg)
{
    UNUSED_PARAMETER(arg);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);

    sys.free_heap = xPortGetFreeHeapSize();
    
    while(1)
    {
        switch(sys.state)
        {
            case SYSTEM_INITIALIZATION:
                init_system_startup();

                sys.initialized = true;
                sys.state = SYSTEM_RUN;
                break;

            case SYSTEM_RUN:
                app_battery();
                app_heart_rate();
                // app_settings();
                app_haptic();

#if (FEATURE_BMA_IMU)
                app_accel();
#endif /* FEATURE_BMA_IMU */
                 app_sensor_update_display();
                break;
            
            case SYSTEM_SLEEP:
            //    sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
            //    if(ble.connection_active == false) {
            //        sd_power_system_off();
            //    }
                sys.state = SYSTEM_RUN;  // temp
                break;

        default:
                break;
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
