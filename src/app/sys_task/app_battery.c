//
// Created by jack on 11/8/20.
//

#include "app_battery.h"
#include "battery.h"
#include "haptic.h"  // todo move enum to common
#include <stdint.h>
#include <stdbool.h>

// FreeRTOS files
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"


extern QueueHandle_t haptic_queue;
extern EventGroupHandle_t charging_event_group;

static void set_battery_status(uint8_t soc);

// App
void run_battery_app(void)
{
    update_battery_state();
    bool battery_charging = is_battery_charging();

    if(battery_charging == true)
    {
        EventBits_t set_charging_bits = BATTERY_CHARGING;
        if(get_battery_prev_charging() == false)
        {
            set_charging_bits |= BATTERY_CHARGING_STARTED;
            eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
            xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(10));
        }
        xEventGroupClearBits(charging_event_group, BATTERY_DISCHARGE);
        xEventGroupSetBits(charging_event_group, set_charging_bits);
    }
    else
    {
        EventBits_t set_charging_bits = BATTERY_DISCHARGE;
        if(get_battery_prev_charging() == true)
        {
            set_charging_bits |= BATTERY_CHARGING_STOPPED;
            eHaptic_State haptic_request = HAPTIC_PULSE_START_STOP_CHARGING;
            xQueueSend(haptic_queue, &haptic_request, pdMS_TO_TICKS(10));

        }
        xEventGroupClearBits(charging_event_group, (BATTERY_CHARGING | BATTERY_CHARGING_STARTED));
        xEventGroupSetBits(charging_event_group, set_charging_bits);
    }

    set_battery_prev_charging(battery_charging);
    set_battery_status(get_battery_soc());

    NRF_LOG_INFO("SOC: %d", get_battery_soc());
    NRF_LOG_INFO("Voltage: %d mV", get_battery_voltage_mv());
    NRF_LOG_INFO("Charging: %d", battery_charging);
}

void set_battery_status(uint8_t soc)
{
    uint32_t status_mask = BATTERY_STATUS_LOW | BATTERY_STATUS_MEDIUM | BATTERY_STATUS_HIGH | BATTERY_STATUS_FULL;

    if(soc <= BATTERY_SOC_LOW)
    {
        xEventGroupClearBits(charging_event_group, status_mask ^ BATTERY_STATUS_LOW);
        xEventGroupSetBits(charging_event_group, BATTERY_STATUS_LOW);
    }
    else if(soc <= BATTERY_SOC_MED)
    {
        xEventGroupClearBits(charging_event_group, status_mask ^ BATTERY_STATUS_MEDIUM);
        xEventGroupSetBits(charging_event_group, BATTERY_STATUS_MEDIUM);
    }
    else if(soc <= BATTERY_SOC_HIGH)
    {
        xEventGroupClearBits(charging_event_group, status_mask ^ BATTERY_STATUS_HIGH);
        xEventGroupSetBits(charging_event_group, BATTERY_STATUS_HIGH);
    }
    else if(soc >= BATTERY_SOC_HIGH)
    {
        xEventGroupClearBits(charging_event_group, status_mask ^ BATTERY_STATUS_HIGH);
        xEventGroupSetBits(charging_event_group, BATTERY_STATUS_FULL);
    }
}