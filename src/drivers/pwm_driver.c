//
// Created by jack on 10/20/20.
//

#include "pwm_driver.h"
#include "app_config.h"


APP_PWM_INSTANCE(HAPTIC_PWM, 1);  // Setup a PWM instance with TIMER 1

// Private Functions
static const app_pwm_t * pwm_get_pwm_instance(ePWM_INDEX index)
{
    const app_pwm_t * ret = NULL;
    switch (index) {
    case HAPTIC_PWM_INDEX:
        ret = &HAPTIC_PWM;
    }
    return ret;
}

NRF_PWM_Type * pwm_get_pwm_base(ePWM_INDEX index)
{
    NRF_PWM_Type * ret = NULL;

    switch(index)
    {
    case HAPTIC_PWM_INDEX:
        ret = HAPTIC_PWM_BASE;
        break;
    default:
        ret = NULL;
    }
    return ret;
}


// Public Functinos

void config_pwm(void)
{
    app_pwm_config_t HAPTIC_PWM_cfg = {
        .pins               = {HAPTIC_PIN, APP_PWM_NOPIN},
        .pin_polarity       = {APP_PWM_POLARITY_ACTIVE_HIGH, APP_PWM_POLARITY_ACTIVE_LOW},
        .num_of_channels    = 1,
        .period_us          = HAPTIC_PERIOD_MICROSECONDS
    };

    ret_code_t err_code = app_pwm_init(&HAPTIC_PWM, &HAPTIC_PWM_cfg, NULL);
    APP_ERROR_CHECK(err_code);

    app_pwm_channel_duty_set(&HAPTIC_PWM, 0, 50);
    app_pwm_enable(&HAPTIC_PWM);
}

void pwm_set_duty_cycle(ePWM_INDEX index, uint8_t duty_cycle)
{
    uint8_t dc = duty_cycle;
    if(duty_cycle == 0)
    {
        dc = 0;
    }
    else if(duty_cycle > 100)
    {
        dc = 100;
    }
    nrf_pwm_enable(pwm_get_pwm_base(index));
    app_pwm_channel_duty_set(pwm_get_pwm_instance(index), 0, dc);
}

uint8_t pwm_get_duty_cycle(ePWM_INDEX index)
{
    return app_pwm_channel_duty_get(pwm_get_pwm_instance(index), 0);
}

void pwm_disable(ePWM_INDEX index)
{
    nrf_pwm_disable(pwm_get_pwm_base(index));
    app_pwm_disable(pwm_get_pwm_instance(index));
}
