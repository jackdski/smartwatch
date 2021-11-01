//
// Created by jack on 6/14/20.
//

#include "battery.h"
#include "SGM40561.h"
#include <stdint.h>
#include <stdbool.h>


#define LOW_POWER_BOUND_PERCENTAGE      20  // [%]

// Private Variables
static Battery_t battery =
{
    .state                  = BATTERY_STATE_UNKNOWN,
    .lowPower               = false,
    .powerPresent           = false,
    .charging               = false,
    .chargingPrevious       = false,
    .soc                    = 0U,
    .voltage_mv             = 0U,
    .dischargeMaxVoltage_mv = 3900U,
    .chargingMaxVoltage_mv  = 4200U,
    .minVoltage_mv          = 3500U,
    .charging_offset_mv     = 300U
};


// Private Functions

static inline void update_battery_voltage_mv(void)
{
    battery.voltage_mv = SGM40562_sample_battery_mv();
}

static inline void update_battery_powerPresent(void)
{
    battery.powerPresent = SGM40561_is_power_present();
}

static inline void update_battery_charging_state(void)
{
    battery.charging = SGM40561_is_charging();
}

uint8_t battery_estimate_soc(void)
{
    uint8_t soc;
    uint16_t numerator = (uint16_t)(battery.voltage_mv - battery.minVoltage_mv);
    uint16_t denominator = (uint16_t)(battery.dischargeMaxVoltage_mv - battery.minVoltage_mv);

    if((battery.charging == true) || (battery.state == BATTERY_STATE_CHARGE_COMPLETE))
    {
        soc = (uint8_t)((numerator * 100) / (denominator + battery.charging_offset_mv));
    }
    else
    {
        soc = (uint8_t)((numerator * 100) / denominator);
    }
    return soc;
}

// Public Functions

void battery_update(void)
{
    update_battery_voltage_mv();
    update_battery_charging_state();

    if(battery.charging == true)
    {
        battery.state = BATTERY_STATE_CHARGING;

        // keep updating how high the voltage can go
        if(battery.voltage_mv > battery.dischargeMaxVoltage_mv)
        {
            battery.dischargeMaxVoltage_mv = battery.voltage_mv;
        }
    }
    else
    {
        if((battery.state == BATTERY_STATE_CHARGING) && (battery.soc == 100))
        {
            battery.dischargeMaxVoltage_mv = battery.voltage_mv;
            battery.charging_offset_mv = battery.chargingMaxVoltage_mv - battery.dischargeMaxVoltage_mv;
        }
        battery.state = BATTERY_STATE_DISCHARGE;

        // keep updating how low the voltage can go
        if(battery.minVoltage_mv < battery.voltage_mv)
        {
            battery.minVoltage_mv = battery.voltage_mv;
        }
    }

    // SOC
    battery.soc = battery_estimate_soc();
    if(battery.soc < LOW_POWER_BOUND_PERCENTAGE)
    {
        battery.lowPower = true;
    }
}

bool battery_is_soc_valid(uint8_t soc)
{
    return ((soc >= 0) && (soc <= 100));
}

bool is_charging_complete(void)
{
    return (battery.state == BATTERY_STATE_CHARGE_COMPLETE);
}

/*
 * Getters & Setters
*/
inline uint8_t get_battery_soc(void)
{
    return battery.soc;
}

inline uint8_t get_battery_voltage_mv(void)
{
    return battery.voltage_mv;
}

inline bool is_battery_charging(void)
{
    return battery.charging;
}

inline void set_battery_chargingPrevious(bool chargingPrevious)
{
    battery.chargingPrevious = chargingPrevious;
}

inline bool get_battery_chargingPrevious(void)
{
    return battery.chargingPrevious;
}

inline bool get_battery_lowPower(void)
{
    return battery.lowPower;
}
