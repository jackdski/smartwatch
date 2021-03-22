//
// Created by jack on 8/18/20.
//

#include "CST816S.h"
#include "twi_driver.h"
#include "nrf_gpio.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"

#include "FreeRTOS.h"
#include "task.h"

CST816S_Event_t * event;


uint8_t CST816S_init(void)
{
    nrf_gpio_pin_set(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));
    nrf_gpio_pin_clear(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(20));
    nrf_gpio_pin_set(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));

//    uint8_t temp;
//    twi_reg_read(CST816S_ADDR, CST816S_REG_CHIP_ID, &temp);
//    vTaskDelay(pdMS_TO_TICKS(5));
//    twi_reg_read(CST816S_ADDR, CST816S_REG_CHIP_ID2, &temp);

//    twi_reg_read(CST816S_ADDR, 0x15, &temp);
//    vTaskDelay(pdMS_TO_TICKS(5));
//    twi_reg_read(CST816S_ADDR, 0xA7, &temp);
    return true;
}

bool CST816S_read_touch(void)
{
    uint8_t data[10];
    twi_rx(CST816S_ADDR, 0, data, 8);
    event->numTouchPoints = data[CST816S_TOUCH_INDEX] & 0x0F;
    // uint8_t pointID = data[data[5]] >> 4;

    if(event->numTouchPoints == 0)
    {
        event->touch_active = false;
        event->gestureID = NO_GESTURE;
    }
    else
    {
        event->touch_active = true;
//        event->point.x = ((uint16_t)(data[CST816S_X_MSB] & 0x0F) << 8) | (data[CST816S_X_LSB]);
//        event->point.y = ((uint16_t)(data[CST816S_Y_MSB] & 0x0F) << 8) | (data[CST816S_Y_LSB]);
        event->point.x = CST816S_SET_MSB(data[CST816S_X_MSB]) | (data[CST816S_X_LSB]);
        event->point.y = CST816S_SET_MSB(data[CST816S_Y_MSB]) | (data[CST816S_Y_LSB]);
        event->gestureID = (uint16_t)data[CST816S_GESTURE_ID_INDEX] >> 6;

        NRF_LOG_INFO("GestureID: %d", event->gestureID);
        NRF_LOG_INFO("X: %i", event->point.x);
        NRF_LOG_INFO("Y: %i", event->point.x);
    }

    return event->touch_active;
}

bool CST816S_get_touch_active(void)
{
    return event->touch_active;
}

void CST816S_get_xy(uint16_t * x, uint16_t * y)
{
    *x = event->point.x;
    *y = event->point.y;
}

/**
 * @description: Puts the touch controller to sleep
 *
 * Should only be used when no touch inputs are expected or needed
 */
void CST816S_sleep(void)
{
    nrf_gpio_pin_clear(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(5));
    nrf_gpio_pin_set(TP_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));
    uint8_t msg[] = {CST816S_POWER_MODE_ADDR, CST816S_POWER_MODE_SLEEP};
    twi_tx(CST816S_ADDR, msg, sizeof(msg));
}
