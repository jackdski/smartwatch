//
// Created by jack on 10/20/20.
//

#ifndef JD_SMARTWATCH_SRC_DRIVERS_PWM_DRIVER_H
#define JD_SMARTWATCH_SRC_DRIVERS_PWM_DRIVER_H

// nRF files
#include "nrf_pwm.h"
#include "app_pwm.h"

#include <stdint.h>

typedef enum {
  HAPTIC_PWM_INDEX
} ePWM_INDEX;

void config_pwm(void);
void pwm_set_duty_cycle(ePWM_INDEX index, uint8_t duty_cycle);
uint8_t pwm_get_duty_cycle(ePWM_INDEX index);
void pwm_disable(ePWM_INDEX index);

#endif //JD_SMARTWATCH_SRC_DRIVERS_PWM_DRIVER_H
