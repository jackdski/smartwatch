//
// Created by jack on 8/18/20.
//

#include "CST816S.h"
#include "twi_driver.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

#include "app_config.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


CST816S_t dev = {
    .point = {
        .x = 0,
        .y = 0
    },
    .gesture        = GESTURE_NONE,
    .prev_gesture   = GESTURE_NONE,
    .numTouchPoints = 0,
    .pressure       = 0,
    .touch_active   = false,
    .asleep         = false
    // .i2c_read       = twi_rx
};


uint8_t init_CST816S(void)
{
    dev.timer = xTimerCreate("CST816S Timer",
                             50U,
                             pdFALSE,
                             NULL,
                             NULL
                             );

    nrf_gpio_pin_set(TP_RESET_PIN);
    xTimerStart(dev.timer, 0);                             

//    uint8_t temp;
//    twi_reg_read(CST816S_ADDR, CST816S_REG_CHIP_ID, &temp);
//    vTaskDelay(pdMS_TO_TICKS(5));
//    twi_reg_read(CST816S_ADDR, CST816S_REG_CHIP_ID2, &temp);

//    twi_reg_read(CST816S_ADDR, 0x15, &temp);
//    vTaskDelay(pdMS_TO_TICKS(5));
//    twi_reg_read(CST816S_ADDR, 0xA7, &temp);
    return true;
}

void init_CST816S_interrupt(void * irq_pfn)
{
    ret_code_t err_code;
    if(nrf_drv_gpiote_is_init() == false)
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    nrf_drv_gpiote_in_config_t input_config;
    input_config.pull = NRF_GPIO_PIN_NOPULL;
    input_config.is_watcher = false;
    input_config.hi_accuracy = true;
    input_config.skip_gpio_setup = false,
    input_config.pull = NRF_GPIO_PIN_PULLUP;
    input_config.sense = NRF_GPIOTE_POLARITY_HITOLO;

    err_code = nrf_drv_gpiote_in_init(TP_INT_PIN, &input_config, irq_pfn);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(TP_INT_PIN, true);
}

void app_CST816S(void)
{
    switch (dev.state)
    {
        case CST816S_INIT_1:
            if (xTimerIsTimerActive(dev.timer) == false)
            {
                nrf_gpio_pin_clear(TP_RESET_PIN);
                xTimerReset(dev.timer, 0);
                dev.state = CST816S_INIT_2;
            }
            break;
        case CST816S_INIT_2:
            if (xTimerIsTimerActive(dev.timer) == false)
            {
                nrf_gpio_pin_set(TP_RESET_PIN);
                xTimerReset(dev.timer, 0);
                dev.state = CST816S_INIT_2;
            }
            break;
        case CST816S_INIT_3:
            if (xTimerIsTimerActive(dev.timer) == false)
            {
                nrf_gpio_pin_clear(TP_RESET_PIN);
                xTimerReset(dev.timer, 0);
                dev.state = CST816S_RUNNING;
            }
            break;
        case CST816S_RUNNING:
        default:
            // nothing
            break;
    }
}

bool CST816S_read_touch(void)
{
    uint8_t data[10];
    twi_rx(CST816S_ADDR, 0, data, 8);
    dev.numTouchPoints = data[CST816S_TOUCH_INDEX] & 0x0F;
    // uint8_t pointID = data[data[5]] >> 4;

    if(dev.numTouchPoints == 0)
    {
        dev.touch_active = false;
        dev.gesture = GESTURE_NONE;
    }
    else
    {
        dev.touch_active = true;
        dev.gesture = data[CST816S_GESTURE_ID_INDEX] >> 6;
        dev.touch_event = data[CST816S_EVENT_INDEX] & EVENT_MASK;
        dev.point.x = (data[CST816S_X_MSB] << 8) | (data[CST816S_X_LSB]);
        dev.point.y = (data[CST816S_Y_MSB] << 8) | (data[CST816S_Y_LSB]);
    }

    return dev.touch_active;
}

bool CST816S_get_touch_active(void)
{
    return dev.touch_active;
}

uint8_t CST816S_get_touch_num(void)
{
    return dev.numTouchPoints;
}

void CST816S_get_xy(int16_t * x, int16_t * y)
{
    *x = dev.point.x;
    *y = dev.point.y;
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
