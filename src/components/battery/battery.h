//
// Created by jack on 6/14/20.
//

#ifndef JDSMARTWATCHPROJECT_BATTERY_H
#define JDSMARTWATCHPROJECT_BATTERY_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  BATTERY_STATE_UNKNOWN,
  BATTERY_STATE_DISCHARGE,
  BATTERY_STATE_CHARGING,
  BATTERY_STATE_CHARGE_COMPLETE
} eBattery_State;

typedef struct
{
  eBattery_State        state;
  bool                  lowPower:1;
  bool                  powerPresent:1;
  bool                  charging:1;
  bool                  chargingPrevious:1;
  uint8_t               soc;
  uint16_t              voltage_mv;             // [mV]
  uint16_t              dischargeMaxVoltage_mv;        // [mV]
  uint16_t              chargingMaxVoltage_mv; // [mV]
  uint16_t              minVoltage_mv;       // [mV]
  uint16_t              charging_offset_mv;     // [mV]
} Battery_t;


/** SOC  **/

/*
 * 20 points, starting at 0% (3500mV) and increasing
 * by 5% to 100% (3900mV)
 *
 * Derived from the battery discharge curve graph found at:
 *      https://forum.pine64.org/showthread.php?tid=8147
 */


/** FUNCTIONS  **/
// Private Functions


// Public Functions
void run_battery_app(void);
void battery_update(void);
uint8_t battery_estimate_soc(void);
uint8_t get_battery_soc(void);
uint8_t get_battery_voltage_mv(void);
bool is_battery_charging(void);
bool is_charging_complete(void);
void set_battery_chargingPrevious(bool chargingPrevious);
bool get_battery_chargingPrevious(void);

bool get_battery_lowPower(void);
bool battery_is_soc_valid(uint8_t soc);

#endif //JDSMARTWATCHPROJECT_BATTERY_H
