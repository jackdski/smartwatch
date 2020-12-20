//
// Created by jack on 6/14/20.
//

#ifndef JDSMARTWATCHPROJECT_RESOURCES_H
#define JDSMARTWATCHPROJECT_RESOURCES_H

#include <stdio.h>
#include "boards.h"
#include "app_config.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "common.h"

/** GPIOTE **/
//void config_gpio_interrupts(void);
void gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void stop_rtos_timers(void);

#endif //JDSMARTWATCHPROJECT_RESOURCES_H
