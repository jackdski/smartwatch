#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_config.h"

#include "side_button.h"
#include "CST816S.h"
#include "bma421.h"


#include "FreeRTOS.h"
#include "task.h"

/** TASK HANDLES **/
extern TaskHandle_t thDisplay;


/** GPIO INTERRUPTS **/
void gpio_irq_callback(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    UNUSED_PARAMETER(action);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    switch(pin)
    {
    case PUSH_BUTTON_IN_PIN:
        button_irq_callback();
        break;
    
    case TP_INT_PIN:
        xTaskNotifyFromISR(thDisplay, 1, eSetBits, &xHigherPriorityTaskWoken);
        break;

#if (BMA_IMU_ENABLED == 1)
    case BMA421_INT_PIN:
        NRF_LOG_INFO("BMA421 INT");
        system_button_handler();
        xTaskNotifyFromISR(thSysTask, 0xFF, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
#endif
    
    default:
        break;
    }

}

void init_gpio_interrupts(void)
{
    init_side_button(&gpio_irq_callback);
    init_CST816S_interrupt(&gpio_irq_callback);
    init_bma_gpio_interrupt(&gpio_irq_callback);
}
