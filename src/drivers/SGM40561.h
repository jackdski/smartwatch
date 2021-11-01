//
// Created by jack on 6/13/20.
//

#ifndef JDSMARTWATCHPROJECT_SGM40561_H
#define JDSMARTWATCHPROJECT_SGM40561_H

#include <stdint.h>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_saadc.h"
#include "app_error.h"

#define BATTERY_SAADC_BUFFER_SIZE               8

// Private Functions
//static void saadc_callback(nrf_drv_saadc_evt_t const * p_event);
//static void battery_monitor_sleep(void);
//static void battery_monitor_wakeup(void);

// Public Functions
void config_SGM40561(void);
bool SGM40561_is_power_present(void);
bool SGM40561_is_charging(void);
uint16_t SGM40561_sample_battery_voltage(void);
uint16_t SGM40562_sample_battery_mv(void);

#endif //JDSMARTWATCHPROJECT_SGM40561_H
