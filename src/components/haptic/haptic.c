//
// Created by jack on 9/12/20.
//

#include "haptic.h"
#include "pwm_driver.h"

#include "app_config.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern QueueHandle_t haptic_queue;
extern TimerHandle_t haptic_timer;

static void haptic_set_pwm_duty_cycle(uint8_t duty_cycle);


// Private Variables
static haptic_pulse_config_t haptic_pulse_configs[HAPTIC_PULSE_COUNT] =
{
    [HAPTIC_PULSE_NONE] =
    {
        .strength   = HAPTIC_STRENGTH_INACTIVE,
        .period_ms  = 0U,
        .repeats    = 0U,
    },
    [HAPTIC_PULSE_INITIALIZATION] =
    {
        .strength   = HAPTIC_STRENGTH_MEDIUM,
        .period_ms  = 1500U,
        .repeats    = 0U,
    },
    [HAPTIC_PULSE_TEXT_MSG] =
    {
        .strength   = HAPTIC_STRENGTH_STRONG,
        .period_ms  = 500U,
        .repeats    = 2U,
    },
    [HAPTIC_PULSE_CALL] =
    {
        .strength   = HAPTIC_STRENGTH_STRONG,
        .period_ms  = 100U,
        .repeats    = 3U,
    },
    [HAPTIC_PULSE_ALARM] =
    {
        .strength   = HAPTIC_STRENGTH_MEDIUM,
        .period_ms  = 750U,
        .repeats    = 20U,
    },
    [HAPTIC_PULSE_LOW_BATTERY] =
    {
        .strength   = HAPTIC_STRENGTH_WEAK,
        .period_ms  = 200U,
        .repeats    = 1U,
    },
    [HAPTIC_PULSE_START_STOP_CHARGING] =
    {
        .strength   = HAPTIC_STRENGTH_MEDIUM,
        .period_ms  = 500U,
        .repeats    = 1U,
    },
};

static Haptic_t haptic = {
    .config     = &haptic_pulse_configs[HAPTIC_PULSE_INITIALIZATION],
    .state      = HAPTIC_PULSE_NONE,
    .duty_cycle = 0,
    .repeats    = 0
};

// Private Functions

static void haptic_set_pulse(haptic_pulse_E pulse)
{
    haptic.config = &haptic_pulse_configs[HAPTIC_PULSE_NONE];
    haptic.repeats = haptic.config->repeats;
}

static void haptic_set_pwm_duty_cycle(uint8_t duty_cycle)
{
    haptic.duty_cycle = duty_cycle;
    pwm_set_duty_cycle(HAPTIC_PWM_INDEX, haptic.duty_cycle);
}

static void haptic_private_start(haptic_pulse_E new_state)
{
    haptic.state = new_state;
    haptic_set_pulse(new_state);
    haptic_set_pwm_duty_cycle(haptic.config->strength);
}

static void haptic_private_runPulse(void)
{
    if (haptic.duty_cycle != 0)
    {
        haptic_set_pwm_duty_cycle(0);
        haptic.repeats--;
    }
    else
    {
        haptic_set_pwm_duty_cycle(haptic.config->strength);
    }
}

static void haptic_private_reset(void)
{
    haptic.state = HAPTIC_PULSE_NONE;
    pwm_disable(HAPTIC_PWM_INDEX);
}

// Public Functions

void app_haptic_init(void)
{
    haptic.state = HAPTIC_PULSE_NONE;
}

void app_haptic(void)
{
    haptic_pulse_E request = HAPTIC_PULSE_NONE;
    if(xQueueReceive(haptic_queue, &request, pdMS_TO_TICKS(0)))
    {
        // start if haptic is inactive or request is higher priority
        if((haptic.state == HAPTIC_PULSE_NONE) || (request > haptic.state))
        {
            haptic_private_start(request);
            xTimerChangePeriod(haptic_timer, haptic.config->period_ms, 0);
            xTimerStart(haptic_timer, 0);
        }
    }
}

void app_haptic_request(haptic_pulse_E request_type)
{
    const haptic_pulse_E request = request_type;
    xQueueSend(haptic_queue, &request, 0);
}

void haptic_timer_callback(TimerHandle_t timerx)
{
    UNUSED_PARAMETER(timerx);

    if (haptic.repeats > 1U)
    {
        haptic_private_runPulse();
        xTimerStart(haptic_timer, 5);
    }
    else
    {
        haptic_private_reset();
        xTimerStop(haptic_timer, pdMS_TO_TICKS(100));
    }
}
