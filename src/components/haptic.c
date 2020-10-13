//
// Created by jack on 9/12/20.
//

#include "haptic.h"

// nRF files
#include "nrf_pwm.h"
#include "app_pwm.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


// RTOS Variables
extern TimerHandle_t haptic_timer;
extern SemaphoreHandle_t haptic_mutex;


// Private Variables
static Haptic_t haptic = {
    .state      = HAPTIC_PULSE_NONE,
    .strength   = HAPTIC_STRENGTH_INACTIVE,
    .period     = 0,
    .pulses     = 0,
    .duty_cycle = 0,
    .ticks      = 0
};

APP_PWM_INSTANCE(PWM1, 1); // Setup a PWM instance with TIMER 1


// Private Functions

static void haptic_set_pwm_duty_cycle(uint8_t duty_cycle)
{
    if(duty_cycle == 0)
    {
        app_pwm_channel_duty_set(&PWM1, 0, 0);
        return;
    }

    nrf_pwm_enable(HAPTIC_PWM_BASE);
    haptic.duty_cycle = duty_cycle;
    app_pwm_channel_duty_set(&PWM1, 0, duty_cycle);
}

static uint8_t haptic_get_pwm_duty_cycle(void)
{
    return haptic.duty_cycle;
}


// Public Functions

void haptic_pwm_config(void)
{
    if(xSemaphoreTake(haptic_mutex, pdMS_TO_TICKS(5000)) == pdPASS)
    {
        app_pwm_config_t pwm1_cfg = {
            .pins               = {16, APP_PWM_NOPIN},
            .pin_polarity       = {APP_PWM_POLARITY_ACTIVE_HIGH, APP_PWM_POLARITY_ACTIVE_LOW},
            .num_of_channels    = 1,
            .period_us          = HAPTIC_PERIOD_MICROSECONDS
        };

        ret_code_t err_code = app_pwm_init(&PWM1, &pwm1_cfg, NULL);
        APP_ERROR_CHECK(err_code);

        app_pwm_channel_duty_set(&PWM1, 0, 50);
        app_pwm_enable(&PWM1);
        xSemaphoreGive(haptic_mutex);
    }
}

void haptic_disable(void)
{
    if(xSemaphoreTake(haptic_mutex, pdMS_TO_TICKS(5000)) == pdPASS)
    {
        nrf_pwm_disable(HAPTIC_PWM_BASE);
        app_pwm_disable(&PWM1);
        xSemaphoreGive(haptic_mutex);
    }
    else
    {
        NRF_LOG_INFO("HAPTIC DISABLE FAILED");
    }
}

void haptic_start(eHaptic_State new_state)
{
    if(xSemaphoreTake(haptic_mutex, pdMS_TO_TICKS(0)))
    {
        NRF_LOG_INFO("Haptic Start: %d", new_state);
        switch(new_state)
        {
        case HAPTIC_PULSE_INITIALIZATION:
            haptic.strength = HAPTIC_STRENGTH_MEDIUM;
            haptic.period = pdMS_TO_TICKS(1500);
            haptic.pulses = 1;  // actuation is toggled over course of 2s
            break;
        case HAPTIC_PULSE_TEXT_MSG:
            haptic.strength = HAPTIC_STRENGTH_STRONG;
            haptic.period = pdMS_TO_TICKS(500);
            haptic.pulses = 4;  // actuation is toggled over course of 2s
            break;
        case HAPTIC_PULSE_CALL:
            // This haptic actuation will require consecutive timer starting
            haptic.strength = HAPTIC_STRENGTH_STRONG;
            haptic.period = pdMS_TO_TICKS(1000);
            haptic.pulses = 1;
            break;
        case HAPTIC_PULSE_ALARM:
            haptic.strength = HAPTIC_STRENGTH_MEDIUM;
            haptic.period = pdMS_TO_TICKS(1000);
            haptic.pulses = 20;  // actuation is toggled over course of 10s
            break;
        case HAPTIC_PULSE_LOW_BATTERY:
            haptic.strength = HAPTIC_STRENGTH_WEAK;
            haptic.period = pdMS_TO_TICKS(200);
            haptic.pulses = 1;
            break;
        case HAPTIC_PULSE_START_STOP_CHARGING:
            haptic.strength = HAPTIC_STRENGTH_MEDIUM;
            haptic.period = pdMS_TO_TICKS(500);
            haptic.pulses = 1;
            break;
        case HAPTIC_PULSE_NONE:
        default:
            haptic.strength = HAPTIC_STRENGTH_INACTIVE;
            haptic.period = 0;
            haptic.pulses = 0;
            haptic.state = HAPTIC_PULSE_NONE;
            haptic_disable();
            return;
        }

        haptic.state = new_state;
        haptic_set_pwm_duty_cycle(haptic.strength);

        xTimerChangePeriod(haptic_timer, haptic.period, 10);
        xTimerStart(haptic_timer, 10);
        xSemaphoreGive(haptic_mutex);
    }
    else
    {
        NRF_LOG_INFO("Haptic failed to start: %d", new_state);
    }
}

void haptic_timer_callback(TimerHandle_t timerx)
{
    UNUSED_PARAMETER(timerx);

    if(haptic.pulses > 1)
    {
        if(haptic_get_pwm_duty_cycle() != 0)
        {
            haptic_set_pwm_duty_cycle(0);
        }
        else
        {
            haptic_set_pwm_duty_cycle(haptic.strength);
        }
        haptic.pulses--;
        xTimerStart(haptic_timer, 5);
    }
    else
    {
        // reset haptic
        haptic.strength = HAPTIC_STRENGTH_INACTIVE;
        haptic.period = 0;
        haptic.pulses = 0;
        haptic.state = HAPTIC_PULSE_NONE;
        haptic_disable();
        xTimerStop(haptic_timer, pdMS_TO_TICKS(100));
    }
}
