//
// Created by jack on 11/8/20.
//

#include "app_sensors.h"
#include "battery.h"
#include "bma421.h"
#include "haptic.h"
#include "HRS3300.h"

#include <string.h>  // memcpy

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// RTOS Variables
extern TaskHandle_t thDisplay;
extern TaskHandle_t thSysTask;
extern QueueHandle_t display_sensor_info_queue;
extern QueueHandle_t haptic_queue;


static SensorData_t sensor_data = {
    .imu = {
        .interrupt_active   = false,
        .interrupt_source   = 0x08,
        .steps              = 0
    },
    .hrs = {
        .channel            = 0,
        .heart_rate         = 0
    },
    .battery = {
        .soc                = 0,
        .level              = BATTERY_LEVEL_DEFAULT,
        .charging_state     = BATTERY_CHARGING_IDLE
    }
};

// Private Functions
static void set_battery_status(uint8_t soc)
{
    if(soc <= BATTERY_SOC_LOW)
    {
        sensor_data.battery.level = BATTERY_LEVEL_LOW;
    }
    else if(soc <= BATTERY_SOC_MED)
    {
        sensor_data.battery.level = BATTERY_LEVEL_MEDIUM;     
    }
    else if(soc <= BATTERY_SOC_HIGH)
    {
        sensor_data.battery.level = BATTERY_LEVEL_HIGH;
    }
    else if(soc >= BATTERY_SOC_HIGH)
    {
        sensor_data.battery.level = BATTERY_LEVEL_FULL;
    }
    else
    {
        // nothing
    }
}


// Apps
void app_accel(void)
{
    update_step_count();
    sensor_data.imu.steps = get_step_count();

    // alternate between wrist wear int. and single tap int.
    sensor_data.imu.interrupt_source ^= 0x08;
    bma423_set_interrupt_source(sensor_data.imu.interrupt_source);
    if(bma423_get_interrupt_status())
    {
        vTaskResume(thDisplay);
    }
}

void app_battery(void)
{
    update_battery_state();
    const bool battery_charging = is_battery_charging();

    if(is_charging_complete())
    {
        sensor_data.battery.charging_state = BATTERY_CHARGING_COMPLETE;
        eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
        xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(0));
    }
    else
    {
        if(battery_charging == true)
        {
            sensor_data.battery.charging_state = BATTERY_CHARGING_ACTIVE;

            if(get_battery_prev_charging() == false)
            {            
                eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
                xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(0));
            }
        }
        else
        {
            sensor_data.battery.charging_state = BATTERY_CHARGING_IDLE;

            if(get_battery_prev_charging() == true)
            {            
                eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
                xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(0));
            }
        }
    }

    set_battery_prev_charging(battery_charging);
    set_battery_status(get_battery_soc());
}

void app_heart_rate(void)
{
    sensor_data.hrs.channel ^= 1;  // flip channels
    HRS3300_enable(true);
    sensor_data.hrs.heart_rate = HRS3300_get_sample(sensor_data.hrs.channel);
    HRS3300_enable(false);
}

void app_sensor_update_display(void)
{
    SensorData_t data;
    memcpy(&data, &sensor_data, sizeof(SensorData_t));

    xQueueSend(display_sensor_info_queue, &data, 5);
}