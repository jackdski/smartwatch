
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

// RTOS Variables
extern TimerHandle_t button_debounce_timer;
extern SemaphoreHandle_t button_semphr;

extern TaskHandle_t thDisplay;
extern TaskHandle_t thSysTask;


void init_side_button(void * irq_pfn)
{
    ret_code_t err_code;
    if(!nrf_drv_gpiote_is_init())
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

    // Button
    err_code = nrf_drv_gpiote_in_init(PUSH_BUTTON_IN_PIN, &input_config, irq_pfn);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(PUSH_BUTTON_IN_PIN, true);
}

void button_irq_callback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStartFromISR(button_debounce_timer, &xHigherPriorityTaskWoken);
}

void button_debounce_callback(TimerHandle_t xTimer)
{
    xTimerStop(button_debounce_timer, 0);

    if(nrf_gpio_pin_read(PUSH_BUTTON_IN_PIN) == false)  // active low
    {
        // Resume tasks if suspended
        if(eTaskGetState(thSysTask) == eSuspended)
        {
            vTaskResume(thSysTask);
        }

        if(eTaskGetState(thDisplay) == eSuspended)
        {
            vTaskResume(thDisplay);
        }

        xSemaphoreGive(button_semphr);
    }
    else
    {
        // nothing
    }
}