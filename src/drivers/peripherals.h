//
// Created by jack on 8/22/20.
//

#ifndef BLINKYEXAMPLEPROJECT_SRC_DRIVERS_PINS_H
#define BLINKYEXAMPLEPROJECT_SRC_DRIVERS_PINS_H

#include "nrf52.h"
#include "nrf_gpio.h"
#include "nrf_spi.h"
#include "nrf_twim.h"
#include "nrf_drv_gpiote.h"

#include "spi_driver.h"
#include "twi_driver.h"
#include "pwm_driver.h"
#include "side_button.h"
#include "SGM40561.h"

void config_pinout(void);
void config_peripherals(void);
void sleep_peripherals(void);

#endif //BLINKYEXAMPLEPROJECT_SRC_DRIVERS_PINS_H
