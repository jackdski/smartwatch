//
// Created by jack on 9/12/20.
//

#include "haptic.h"
#include "drivers/pwm_driver.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// Private Variables
static Haptic_t haptic = {
    .state      = HAPTIC_PULSE_NONE,
    .strength   = HAPTIC_STRENGTH_INACTIVE,
    .period_ms     = 0,
    .pulses     = 0,
    .duty_cycle = 0,
    .ticks      = 0
};


// Private Functions

static void haptic_set_pwm_duty_cycle(uint8_t duty_cycle)
{
    haptic.duty_cycle = duty_cycle;
    pwm_set_duty_cycle(HAPTIC_PWM_INDEX, haptic.duty_cycle);
}

static uint8_t haptic_get_pwm_duty_cycle(void)
{
    return haptic.duty_cycle;
}


// Public Functions

void haptic_disable(void)
{
    pwm_disable(HAPTIC_PWM_INDEX);
}

void haptic_start(eHaptic_State new_state)
{
    switch(new_state)
    {
    case HAPTIC_PULSE_INITIALIZATION:
        haptic.strength = HAPTIC_STRENGTH_MEDIUM;
        haptic.period_ms = 1500;
        haptic.pulses = 1;  // actuation is toggled over course of 2s
        break;
    case HAPTIC_PULSE_TEXT_MSG:
        haptic.strength = HAPTIC_STRENGTH_STRONG;
        haptic.period_ms = 500;
        haptic.pulses = 4;  // actuation is toggled over course of 2s
        break;
    case HAPTIC_PULSE_CALL:
        // This haptic actuation will require consecutive timer starting
        haptic.strength = HAPTIC_STRENGTH_STRONG;
        haptic.period_ms = 1000;
        haptic.pulses = 1;
        break;
    case HAPTIC_PULSE_ALARM:
        haptic.strength = HAPTIC_STRENGTH_MEDIUM;
        haptic.period_ms = 1000;
        haptic.pulses = 20;  // actuation is toggled over course of 10s
        break;
    case HAPTIC_PULSE_LOW_BATTERY:
        haptic.strength = HAPTIC_STRENGTH_WEAK;
        haptic.period_ms = 200;
        haptic.pulses = 1;
        break;
    case HAPTIC_PULSE_START_STOP_CHARGING:
        haptic.strength = HAPTIC_STRENGTH_MEDIUM;
        haptic.period_ms = 500;
        haptic.pulses = 1;
        break;
    case HAPTIC_PULSE_NONE:
    default:
        haptic.strength = HAPTIC_STRENGTH_INACTIVE;
        haptic.period_ms = 0;
        haptic.pulses = 0;
        haptic.state = HAPTIC_PULSE_NONE;
        haptic_disable();
        return;
    }

    haptic.state = new_state;
    haptic_set_pwm_duty_cycle(haptic.strength);
}

void haptic_pulse_run(void)
{
    if(haptic.duty_cycle != 0)
    {
        haptic_set_pwm_duty_cycle(0);
        haptic.pulses--;
    }
    else
    {
        haptic_set_pwm_duty_cycle(haptic.strength);
    }
}

void haptic_reset(void)
{
    haptic.strength = HAPTIC_STRENGTH_INACTIVE;
    haptic.period_ms = 0;
    haptic.pulses = 0;
    haptic.state = HAPTIC_PULSE_NONE;
}

uint16_t haptic_get_period_ms(void)
{
    return haptic.period_ms;
}

uint8_t haptic_get_pulses(void)
{
    return haptic.pulses;
}
