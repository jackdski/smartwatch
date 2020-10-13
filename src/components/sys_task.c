//
// Created by jack on 7/19/20.
//

#include "sys_task.h"
#include "bma421.h"
#include "HRS3300.h"
#include "battery.h"
#include "nrf52.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#include "display/display.h"
#include "resources.h"
#include "drivers/peripherals.h"
#include "haptic.h"

// RTOS Variables
extern QueueHandle_t system_queue;
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
    .charging = false,
    .soc = 0,
    .step_count = 0,
    .battery_voltage = 0
};


/** Private Functions **/
//void add_to_sys_queue(eMessage * msg)
//{
//    if(msg != NO_MSG)
//    {
//        xQueueSend(system_queue, &msg, pdMS_TO_TICKS(5));
//    }
//}

static void run_battery_app(void)
{
//    update_battery_state();
    sys.soc = get_battery_soc();
    sys.charging = get_battery_charging();

    if(sys.charging == true)
    {
        EventBits_t set_charging_bits = BATTERY_CHARGING;
        if(sys.prev_charging == false)
        {
            set_charging_bits |= BATTERY_CHARGING_STARTED;
            haptic_start(HAPTIC_PULSE_START_STOP_CHARGING);
        }
        xEventGroupClearBits(charging_event_group, BATTERY_DISCHARGE);
        xEventGroupSetBits(charging_event_group, set_charging_bits);
    }
    else
    {
        EventBits_t set_charging_bits = BATTERY_DISCHARGE;
        if(sys.prev_charging == true)
        {
            set_charging_bits |= BATTERY_CHARGING_STOPPED;
            haptic_start(HAPTIC_PULSE_START_STOP_CHARGING);
        }
        xEventGroupClearBits(charging_event_group, (BATTERY_CHARGING | BATTERY_CHARGING_STARTED));
        xEventGroupSetBits(charging_event_group, set_charging_bits);
    }

    sys.prev_charging = sys.charging;
    NRF_LOG_INFO("SOC: %d", sys.soc);
    NRF_LOG_INFO("Voltage: %d", sys.battery_voltage);
    NRF_LOG_INFO("Charging: %d", sys.charging);
}

static void get_system_update_results(void)
{
    sys.step_count = get_step_count();
}

static bool is_battery_soc_valid(uint8_t soc)
{
    return ((soc >= 0) && (soc <= 100));
}

static bool init_system_startup(void)
{
    bool bma_initialized = false, HRS3300_initialized = false;
    EventBits_t set_bits = xEventGroupGetBits(component_event_group);

    bma_initialized = init_bma();
    HRS3300_initialized = HRS3300_init();

    haptic_pwm_config();
    haptic_start(HAPTIC_PULSE_INITIALIZATION);
    set_bits |= COMPONENT_HAPTIC;

    if(bma_initialized == true)
    {
        set_bits |= COMPONENT_SENSOR_IMU;
    }
    if(HRS3300_initialized == true)
    {
        set_bits |= COMPONENT_SENSOR_HRS;
    }

    update_battery_state();
    sys.soc = get_battery_soc();
    sys.charging = get_battery_charging();
    if(is_battery_soc_valid(sys.soc))
    {
        set_bits |= COMPONENT_BATTERY_MONITOR;
    }

    // set bits and wait up to 10s for remaining bits to be set before performing a system reset
    bool ret = false;
    xEventGroupSetBits(component_event_group, set_bits);
    EventBits_t returned_bits;
    returned_bits = xEventGroupWaitBits(component_event_group,
                                        COMPONENT_LIST_ALL,
                                        pdTRUE,
                                        pdTRUE,
                                        pdMS_TO_TICKS(10000));

    if((returned_bits & COMPONENT_LIST_ALL) == COMPONENT_LIST_ALL)
    {
        ret = true;
    }
    return ret;
//    return true;
}


/** Public Functions **/
void sys_task(void * arg)
{
    NRF_LOG_INFO("Init SystemTask");
    UNUSED_PARAMETER(arg);

    // Task Local Variables
    // static eMessage message = NO_MSG;

    // System Startup
//    sys.initialized = init_system_startup();
//    if(sys.initialized == false)
//    {
//        EventBits_t initialized_components;
//        initialized_components = xEventGroupGetBits(component_event_group);
//        uint8_t i;
//        for(i = 0; i < NUM_COMPONENTS; i++)
//        {
//            if(!(initialized_components & (1 >> i)))
//            {
//                NRF_LOG_INFO("Component[%d] not initialized", i);
//            }
//        }
//        NRF_LOG_INFO("Some Components were not initialized. Resetting device...");
//        NVIC_SystemReset();
//    }
//    else
//    {
//        // clear bits for later use (sleep)
        xEventGroupClearBits(component_event_group, COMPONENT_LIST_ALL);
//    }
    NRF_LOG_INFO("SysTask Init'd");

    while(1)
    {
//        run_battery_app();
        // run_imu_app();
        // update_step_count();
        // run_heart_rate_app();
        NRF_LOG_INFO("Steps: %d", sys.step_count);
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


//const char * get_systask_name(eMessage msg)
//{
//    switch(msg)
//    {
//    case NO_MSG: return "NO_MSG";
//    case INITIALIZATION: return "INITIALIZATION";
//    case GO_TO_SLEEP: return "GO_TO_SLEEP";
//    case CHARGING_STARTED: return "CHARGING_STARTED";
//    case CHARGING_STOPPED: return "CHARGING_STOPPED";
//    case CHARGING_COMPLETE: return "CHARGING_COMPLETE";
//    case DISPLAY_INACTIVITY_TIMER_EXPIRED: return "DISPLAY_INACTIVITY_TIMER_EXPIRED";
//    case DISPLAY_ACTIVE: return "DISPLAY_ACTIVE";
//    case BUTTON_PRESSED: return "BUTTON_PRESSED";
//    }
//}
