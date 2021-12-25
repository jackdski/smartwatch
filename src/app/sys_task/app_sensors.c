//
// Created by jack on 11/8/20.
//

#include "app_sensors.h"

#include "alerts.h"
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


/** Private Functions **/
static void set_battery_status(uint8_t soc);


static SensorData_t sensor_data = {
    .imu =
    {
        .interrupt_active   = false,
        .interrupt_source   = 0x08,
        .steps              = 0
    },
    .battery =
    {
        .soc                = 0,
        .level              = BATTERY_LEVEL_DEFAULT,
        .prev_level         = BATTERY_LEVEL_DEFAULT,
        .charging_state     = BATTERY_CHARGING_IDLE
    }
};


/** Public Functions **/
void app_accel(void)
{
    bma_updateStepCount();
    sensor_data.imu.steps = bma_getStepCount();

    // alternate between wrist wear int. and single tap int.
    sensor_data.imu.interrupt_source ^= 0x08;
    bma_setInterruptSource(sensor_data.imu.interrupt_source);
    if(bma_getInterruptStatus())
    {
        vTaskResume(thDisplay);
    }
}

void app_battery(void)
{
    battery_update();
    const bool battery_charging = is_battery_charging();

    if(is_charging_complete())
    {
        sensor_data.battery.charging_state = BATTERY_CHARGING_COMPLETE;
        app_haptic_request(HAPTIC_PULSE_START_STOP_CHARGING);
    }
    else
    {
        if(battery_charging == true)
        {
            sensor_data.battery.charging_state = BATTERY_CHARGING_ACTIVE;

            if(get_battery_chargingPrevious() == false)
            {
                app_haptic_request(HAPTIC_PULSE_START_STOP_CHARGING);
            }
        }
        else
        {
            sensor_data.battery.charging_state = BATTERY_CHARGING_IDLE;

            if(get_battery_chargingPrevious() == true)
            {
                app_haptic_request(HAPTIC_PULSE_START_STOP_CHARGING);
            }
        }
    }

    set_battery_chargingPrevious(battery_charging);
    set_battery_status(get_battery_soc());
}



void app_sensor_update_display(void)
{
    SensorData_t data;
    memcpy(&data, &sensor_data, sizeof(SensorData_t));

    xQueueSend(display_sensor_info_queue, &data, 5);
}

// Private Functions
static void set_battery_status(uint8_t soc)
{
    if(soc <= BATTERY_SOC_LOW)
    {
        if(sensor_data.battery.prev_level != BATTERY_LEVEL_LOW)
        {
            set_alert(ALERT_BATTERY_LOW);
        }

        sensor_data.battery.prev_level = sensor_data.battery.level;
        sensor_data.battery.level = BATTERY_LEVEL_LOW;
    }
    else if(soc <= BATTERY_SOC_MED)
    {
        sensor_data.battery.prev_level = sensor_data.battery.level;
        sensor_data.battery.level = BATTERY_LEVEL_MEDIUM;
    }
    else if(soc <= BATTERY_SOC_HIGH)
    {
        sensor_data.battery.prev_level = sensor_data.battery.level;
        sensor_data.battery.level = BATTERY_LEVEL_HIGH;
    }
    else if(soc >= BATTERY_SOC_HIGH)
    {
        if(sensor_data.battery.prev_level != BATTERY_SOC_HIGH)
        {
            set_alert(ALERT_BATTERY_CHARGED);
        }

        sensor_data.battery.prev_level = sensor_data.battery.level;
        sensor_data.battery.level = BATTERY_LEVEL_FULL;
    }
    else
    {
        // nothing
    }
}