//
// Created by jack on 9/12/20.
//

#ifndef HAPTIC_H_
#define HAPTIC_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

typedef enum
{
  HAPTIC_PULSE_NONE,
  HAPTIC_PULSE_INITIALIZATION,

  // Sorted according to priority
  HAPTIC_PULSE_LOW_BATTERY,
  HAPTIC_PULSE_START_STOP_CHARGING,
  HAPTIC_PULSE_TEXT_MSG,
  HAPTIC_PULSE_CALL,
  HAPTIC_PULSE_ALARM,

  HAPTIC_PULSE_COUNT,
} haptic_pulse_E;

typedef enum
{
  HAPTIC_STRENGTH_INACTIVE  = 0,
  HAPTIC_STRENGTH_WEAK      = 25,
  HAPTIC_STRENGTH_MEDIUM    = 50,
  HAPTIC_STRENGTH_STRONG    = 85,
} haptic_strength_E;

typedef struct
{
  haptic_strength_E   strength;
  uint8_t             repeats;
  uint32_t            period_ms;
} haptic_pulse_config_t;


typedef struct
{
  haptic_pulse_config_t * config;
  haptic_pulse_E          state;
  uint8_t                 duty_cycle;
  uint8_t                 repeats;
} Haptic_t;

// App
void app_haptic_init(void);
void app_haptic(void);
void haptic_timer_callback(TimerHandle_t timerx);

// Public Functions
void app_haptic_request(haptic_pulse_E request_type);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_HAPTIC_H
