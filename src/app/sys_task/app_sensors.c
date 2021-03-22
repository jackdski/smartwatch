//
// Created by jack on 11/8/20.
//

#include "app_sensors.h"
#include "battery.h"
#include "bma421.h"
#include "haptic.h"
#include "HRS3300.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// RTOS Variables
extern TaskHandle_t thDisplay;
extern TaskHandle_t thSysTask;
extern QueueHandle_t display_info_queue;
extern QueueHandle_t haptic_queue;


static BatteryEventBits battery_events = 0;

static IMU_t imu = {
    .interrupt_active = false,
    .interrupt_source = 0x08,
    .steps = 0
};

static HRS_t hrs = {
    .channel        = 0,
    .heart_rate     = 0
};

// Private Functions
static void set_battery_status(uint8_t soc)
{
    uint32_t status_mask = BATTERY_STATUS_LOW | BATTERY_STATUS_MEDIUM | BATTERY_STATUS_HIGH | BATTERY_STATUS_FULL;

    if(soc <= BATTERY_SOC_LOW)
    {
        battery_events &= ~(status_mask ^ BATTERY_STATUS_LOW); // clear battery status bits
        battery_events |= BATTERY_STATUS_LOW;
    }
    else if(soc <= BATTERY_SOC_MED)
    {
        battery_events &= ~(status_mask ^ BATTERY_STATUS_MEDIUM);
        battery_events |= BATTERY_STATUS_MEDIUM;
    }
    else if(soc <= BATTERY_SOC_HIGH)
    {
        battery_events &= ~(status_mask ^ BATTERY_STATUS_HIGH);
        battery_events |= BATTERY_STATUS_HIGH;
    }
    else if(soc >= BATTERY_SOC_HIGH)
    {
        battery_events &= ~(status_mask ^ BATTERY_STATUS_HIGH);
        battery_events |= BATTERY_STATUS_FULL;
    }
    else
    {
        // nothing
    }
}


// Apps
void run_accel_app(void)
{
    update_step_count();
    imu.steps = get_step_count();
    NRF_LOG_INFO("Steps: %d", imu.steps);

    // alternate between wrist wear int. and single tap int.
    imu.interrupt_source ^= 0x08;
    bma423_set_interrupt_source(imu.interrupt_source);
    if(bma423_get_interrupt_status())
    {
        NRF_LOG_INFO("ACCEL: Int. Detected");
        vTaskResume(thDisplay);
    }
}

void run_battery_app(void)
{
    update_battery_state();
    const bool battery_charging = is_battery_charging();
    uint32_t set_charging_bits = 0;

    if(battery_charging == true)
    {
        set_charging_bits = BATTERY_CHARGING;
        if(get_battery_prev_charging() == false)
        {
            set_charging_bits |= BATTERY_CHARGING_STARTED;
            eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
            xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(10));
        }
        battery_events &= ~BATTERY_DISCHARGE;
        battery_events |= set_charging_bits;
    }
    else
    {
        set_charging_bits = BATTERY_DISCHARGE;
        if(get_battery_prev_charging() == true)
        {
            set_charging_bits |= BATTERY_CHARGING_STOPPED;
            eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
            xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(10));

        }
        battery_events &= ~(BATTERY_CHARGING | BATTERY_CHARGING_STARTED);
        battery_events |= set_charging_bits;
    }

    set_battery_prev_charging(battery_charging);
    set_battery_status(get_battery_soc());

    NRF_LOG_INFO("SOC: %d", get_battery_soc());
    NRF_LOG_INFO("Voltage: %d mV", get_battery_voltage_mv());
    NRF_LOG_INFO("Charging: %d", battery_charging);
}

void run_heart_rate_app(void)
{
    hrs.channel ^= 1;
    HRS3300_enable(true);
    hrs.heart_rate = HRS3300_get_sample(hrs.channel);
    HRS3300_enable(false);
    NRF_LOG_INFO("Heart rate: %d", hrs.heart_rate);
}

void run_sensor_update_display(void)
{
    SensorData_t data;
    data.steps = imu.steps;
    data.heart_rate = hrs.heart_rate;
    data.battery_soc = get_battery_soc();
    data.battery_events = battery_events;

    xQueueSend(display_info_queue, &data, 5);
}