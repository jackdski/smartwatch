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

  HAPTIC_PULSE_COUNT
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
  haptic_pulse_E     state;
  haptic_pulse_E     request;
  haptic_strength_E   strength;
  uint16_t          period_ms;
  uint8_t           pulses;
  uint8_t           duty_cycle;
  uint16_t          ticks;
} Haptic_t;

// App
void init_haptic(void);
void app_haptic(void);
void haptic_timer_callback(TimerHandle_t timerx);


// Public Functions
void haptic_disable(void);
void haptic_start(haptic_pulse_E new_state);
uint16_t haptic_get_period_ms(void);
uint8_t haptic_get_pulses(void);
void haptic_pulse_run(void);
void haptic_request(haptic_pulse_E request_type);
void haptic_reset(void);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_HAPTIC_H
