//
// Created by jack on 6/14/20.
//

#include "battery.h"
#include "drivers/SGM40561.h"
#include <stdint.h>
#include <stdbool.h>

#define LOW_POWER_BOUND     20  // [%]

// Private Variables
static Battery_t battery_monitor = {
    .state = BATTERY_UNKNOWN_STATE,
    .low_power = false,
    .power_present = false,
    .charging = false,
    .soc = 0,
    .voltage_mv = 0,
    .full_voltage_mv = 3900,
    .full_charge_voltage_mv = 4200,
    .empty_voltage_mv = 3500,
    .charging_offset_mv = 300
};

// Private Functions

// TODO: unit tests
uint8_t estimate_soc(void)
{
    uint8_t soc;
    uint16_t numerator = (uint16_t)(battery_monitor.voltage_mv - battery_monitor.empty_voltage_mv);
    uint16_t denominator = (uint16_t)(battery_monitor.full_voltage_mv - battery_monitor.empty_voltage_mv);

    if((battery_monitor.charging == true) || (battery_monitor.state == CHARGE_COMPLETE))
    {
        soc = (uint8_t)((numerator * 100) / (denominator + battery_monitor.charging_offset_mv));
    }
    else
    {
        soc = (uint8_t)((numerator * 100) / denominator);
    }
    return soc;
}

inline uint8_t get_battery_soc(void)
{
    return battery_monitor.soc;
}

inline uint8_t get_battery_charging(void)
{
    return battery_monitor.charging;
}

inline bool get_battery_low_power(void)
{
    return battery_monitor.low_power;
}


// Public Functions

void update_battery_state(void)
{
    battery_monitor.voltage_mv = battery_monitor_sample();
    battery_monitor.power_present = SGM40561_is_power_present();
    battery_monitor.charging = SGM40561_is_charging();

    if(battery_monitor.charging == true)
    {
        battery_monitor.state = CHARGING;

        // keep updating how high the voltage can go
        if(battery_monitor.voltage_mv > battery_monitor.full_voltage_mv)
        {
            battery_monitor.full_voltage_mv = battery_monitor.voltage_mv;
        }
    }
    else
    {
        // TODO: write unit test for this case
        if((battery_monitor.state == CHARGING) && (battery_monitor.soc == 100))
        {
            battery_monitor.full_voltage_mv = battery_monitor.voltage_mv;
            battery_monitor.charging_offset_mv = battery_monitor.full_charge_voltage_mv - battery_monitor.full_voltage_mv;
        }
        battery_monitor.state = DISCHARGE;

        // keep updating how low the voltage can go
        // TODO: write unit test
        if(battery_monitor.empty_voltage_mv < battery_monitor.voltage_mv)
        {
            battery_monitor.empty_voltage_mv = battery_monitor.voltage_mv;
        }
    }

    // SOC - TODO: unit test
    battery_monitor.soc = estimate_soc();
    if(battery_monitor.soc < LOW_POWER_BOUND)
    {
        battery_monitor.low_power = true;
    }
}

bool is_battery_soc_valid(uint8_t soc)
{
    return ((soc >= 0) && (soc <= 100));
}
