//
// Created by jack on 10/20/20.
//

#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_

// nRF files
#include "nrf_pwm.h"
#include "app_pwm.h"

#include <stdint.h>

typedef enum
{
  HAPTIC_PWM_INDEX
} PWM_Index_E;

void config_pwm(void);
void pwm_set_duty_cycle(PWM_Index_E index, uint8_t duty_cycle);
uint8_t pwm_get_duty_cycle(PWM_Index_E index);
void pwm_disable(PWM_Index_E index);

#endif // PWM_DRIVER_H_
