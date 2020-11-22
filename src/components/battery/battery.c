//
// Created by jack on 6/14/20.
//

#include "battery.h"
#include "SGM40561.h"
#include <stdint.h>
#include <stdbool.h>


#define LOW_POWER_BOUND_PERCENTAGE      20  // [%]

// Private Variables
static Battery_t battery_monitor = {
    .state = BATTERY_UNKNOWN_STATE,
    .low_power = false,
    .power_present = false,
    .charging = false,
    .prev_charging = false,
    .soc = 0,
    .voltage_mv = 0,
    .full_voltage_mv = 3900,
    .full_charge_voltage_mv = 4200,
    .empty_voltage_mv = 3500,
    .charging_offset_mv = 300
};


// Private Functions
inline uint8_t get_battery_soc(void)
{
    return battery_monitor.soc;
}

inline uint8_t get_battery_voltage_mv(void)
{
    return battery_monitor.voltage_mv;
}

inline bool is_battery_charging(void)
{
    return battery_monitor.charging;
}

inline void set_battery_prev_charging(bool prev_charging)
{
    battery_monitor.prev_charging = prev_charging;
}

inline bool get_battery_prev_charging(void)
{
    return battery_monitor.prev_charging;
}

inline bool get_battery_low_power(void)
{
    return battery_monitor.low_power;
}

static inline void update_battery_voltage_mv(void)
{
    battery_monitor.voltage_mv = battery_monitor_sample();
}

static inline void update_battery_power_present(void)
{
    battery_monitor.power_present = SGM40561_is_power_present();
}

static inline void update_battery_charging_state(void)
{
    battery_monitor.charging = SGM40561_is_charging();
}

// Public Functions
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

void update_battery_state(void)
{
    update_battery_voltage_mv();
    update_battery_charging_state();

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
        if((battery_monitor.state == CHARGING) && (battery_monitor.soc == 100))
        {
            battery_monitor.full_voltage_mv = battery_monitor.voltage_mv;
            battery_monitor.charging_offset_mv = battery_monitor.full_charge_voltage_mv - battery_monitor.full_voltage_mv;
        }
        battery_monitor.state = DISCHARGE;

        // keep updating how low the voltage can go
        if(battery_monitor.empty_voltage_mv < battery_monitor.voltage_mv)
        {
            battery_monitor.empty_voltage_mv = battery_monitor.voltage_mv;
        }
    }

    // SOC
    battery_monitor.soc = estimate_soc();
    if(battery_monitor.soc < LOW_POWER_BOUND_PERCENTAGE)
    {
        battery_monitor.low_power = true;
    }
}

bool is_battery_soc_valid(uint8_t soc)
{
    return ((soc >= 0) && (soc <= 100));
}
