//
// Created by jack on 6/14/20.
//

#include "resources.h"
#include "sys_task.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

// FreeRTOS Timers
extern TimerHandle_t display_timeout_tmr;
extern TimerHandle_t display_lv_handler_tmr;
extern TimerHandle_t haptic_timer;

// FreeRTOS Variables
extern QueueHandle_t system_queue;

static Time_t time = {
    .hour = 0,
    .minute = 0,
    .second = 0,
    .day_of_week = 0,
    .month = 0,
    .year = 0
};

/** GPIOTE **/

void gpio_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
//    eMessage msg = BUTTON_PRESSED;

    if(pin == PUSH_BUTTON_IN_PIN)
    {
        NRF_LOG_INFO("Button press!");
//        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//        xQueueSendFromISR(system_queue, &msg, &xHigherPriorityTaskWoken);
    }
}

/** TIME **/
void update_system_time(Time_t new_time)
{
    memcpy(&time, &new_time, sizeof(Time_t));
}

/** FreeRTOS Functions **/
void stop_rtos_timers(void)
{
    xTimerStop(display_timeout_tmr, 5);
    xTimerStop(display_lv_handler_tmr, 5);
    xTimerStop(haptic_timer, 5);
}
