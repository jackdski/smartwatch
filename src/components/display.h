//
// Created by jack on 5/14/20.
//

#ifndef JDSMARTWATCHPROJECT_DISPLAY_H
#define JDSMARTWATCHPROJECT_DISPLAY_H

#include "lvgl/lvgl.h"
#include "drivers/ssd1306.h"

#include "nrf_twi.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#define DISP_BUF_SIZE               (128 * 64 / 8)
#define DISPLAY_TWI_INSTANCE_ID     1

void display_task(void * arg);

void display_init(void);
void display_twi_write_cmd(uint8_t cmd);
void display_turn_on(void);
void display_turn_off(void);
void display_start_send(uint8_t row, uint8_t col);
void display_twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);

#endif //JDSMARTWATCHPROJECT_DISPLAY_H
