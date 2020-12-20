//
// Created by jack on 8/2/20.
//

#include "button.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "nrfx_gpiote.h"

/*
 * Enable the button by driving PUSH_BUTTON_OUT_PIN high
 */
void config_button(void * callback)
{
    ret_code_t err_code;
    if(!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    nrf_gpio_pin_set(PUSH_BUTTON_OUT_PIN);
//    nrf_gpio_cfg_sense_input(PUSH_BUTTON_IN_PIN, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(PUSH_BUTTON_IN_PIN, &in_config, callback);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PUSH_BUTTON_IN_PIN, true);
}

void config_button_event(void * callback)
{
//    if(!nrf_drv_gpiote_is_init())
//    {
//        nrf_drv_gpiote_init();
//    }

//    nrf_drv_gpiote_in_config_t button_gpiote_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
//    button_gpiote_config.pull = NRF_GPIO_PIN_PULLDOWN;
//    nrf_drv_gpiote_in_init(10,
//                           &button_gpiote_config,
//                           callback);
}

