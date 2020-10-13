//
// Created by jack on 6/14/20.
//

#ifndef JDSMARTWATCHPROJECT_BATTERY_H
#define JDSMARTWATCHPROJECT_BATTERY_H

#include <stdint.h>
#include "drivers/SGM40561.h"

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
//static uint8_t estimate_soc(void);

// Public Functions
void update_battery_state(void);
uint8_t estimate_soc(void);
uint8_t get_battery_soc(void);
uint8_t get_battery_charging(void);
bool get_battery_low_power(void);

#endif //JDSMARTWATCHPROJECT_BATTERY_H
