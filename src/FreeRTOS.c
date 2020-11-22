//
// Created by jack on 5/19/20.
//

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

/** TASK HANDLES **/
TaskHandle_t m_logger_thread;                           /**< Definition of Logger thread. */
TaskHandle_t thLED;
TaskHandle_t thSysTask;
TaskHandle_t thBLEMan;
TaskHandle_t thDisplay;

/** EVENT GROUPS **/
EventGroupHandle_t component_event_group;
EventGroupHandle_t charging_event_group;

/** TIMER HANDLES **/
TimerHandle_t display_timeout_tmr;
TimerHandle_t haptic_timer;

/** QUEUES **/
QueueHandle_t system_queue;
QueueHandle_t settings_queue;
QueueHandle_t display_queue;
QueueHandle_t haptic_queue;
QueueHandle_t ble_action_queue;
QueueHandle_t ble_response_queue;

/** SEMAPHORES **/
SemaphoreHandle_t twi_mutex;
SemaphoreHandle_t spi_mutex;
SemaphoreHandle_t lvgl_mutex;
SemaphoreHandle_t haptic_mutex;
SemaphoreHandle_t button_semphr;

