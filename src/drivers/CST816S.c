//
// Created by jack on 8/18/20.
//

#include "CST816S.h"
#include "twi_driver.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

uint8_t data[63];

void CST816S_init(void)
{
    nrf_gpio_pin_set(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));
    nrf_gpio_pin_clear(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(5));
    nrf_gpio_pin_set(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));

    uint8_t temp;
    twi_read_reg(CST816S_ADDR, 0x15, &temp, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
    twi_read_reg(CST816S_ADDR, 0xA7, &temp, 1);
}

CST816S_Event_t * CST816S_read_touch(void)
{
    CST816S_Event_t * evt = {0};
    twi_read_reg(CST816S_ADDR, 0, data, 63);

    evt->numTouchPoints = data[2] & 0x0F;
//    uint8_t pointID = data[];

    return evt;
}