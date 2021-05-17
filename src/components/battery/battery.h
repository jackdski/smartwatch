//
// Created by jack on 6/14/20.
//

#ifndef JDSMARTWATCHPROJECT_BATTERY_H
#define JDSMARTWATCHPROJECT_BATTERY_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  BATTERY_UNKNOWN_STATE,
  DISCHARGE,
  CHARGING,
  CHARGE_COMPLETE
} eBattery_State;

typedef struct {
  eBattery_State        state;
  bool                  low_power:1;
  bool                  power_present:1;
  bool                  charging:1;
  bool                  prev_charging:1;
  uint8_t               soc;
  uint16_t              voltage_mv;             // [mV]
  uint16_t              full_voltage_mv;        // [mV]
  uint16_t              full_charge_voltage_mv; // [mV]
  uint16_t              empty_voltage_mv;       // [mV]
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
void update_battery_state(void);
uint8_t estimate_soc(void);
uint8_t get_battery_soc(void);
uint8_t get_battery_voltage_mv(void);
bool is_battery_charging(void);
bool is_charging_complete(void);
void set_battery_prev_charging(bool prev_charging);
bool get_battery_prev_charging(void);

bool get_battery_low_power(void);
bool is_battery_soc_valid(uint8_t soc);

#endif //JDSMARTWATCHPROJECT_BATTERY_H
