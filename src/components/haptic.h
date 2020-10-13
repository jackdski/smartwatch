//
// Created by jack on 9/12/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_HAPTIC_H
#define BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_HAPTIC_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

typedef enum {
  HAPTIC_PULSE_NONE,
  HAPTIC_PULSE_INITIALIZATION,
  HAPTIC_PULSE_TEXT_MSG,
  HAPTIC_PULSE_CALL,
  HAPTIC_PULSE_ALARM,
  HAPTIC_PULSE_LOW_BATTERY,
  HAPTIC_PULSE_START_STOP_CHARGING,
} eHaptic_State;

typedef enum {
  HAPTIC_STRENGTH_INACTIVE  = 0,
  HAPTIC_STRENGTH_WEAK      = 25,
  HAPTIC_STRENGTH_MEDIUM    = 50,
  HAPTIC_STRENGTH_STRONG    = 85,
} eHapticStrength;

typedef struct {
  eHaptic_State     state;
  eHapticStrength   strength;
  uint16_t          period;
  uint8_t           pulses;
  uint8_t           duty_cycle;
  uint16_t          ticks;
} Haptic_t;


// Private Functions
//static void haptic_set_pwm_duty_cycle(uint8_t duty_cycle);
//static uint8_t haptic_get_pwm_duty_cycle(void);

// Public Functions
void haptic_pwm_config(void);
void haptic_disable(void);
void haptic_timer_callback(TimerHandle_t timerx);
void haptic_start(eHaptic_State new_state);

#endif //BLINKYEXAMPLEPROJECT_SRC_COMPONENTS_HAPTIC_H
