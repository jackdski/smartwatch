/*
 * @author  jdanielski
 * @file    main.c
 */

// nRF includes
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "app_error.h"
#include "nrf_gpiote.h"

// nRF driver files
#include "nrf_sdh_soc.h"
#include "nrf_sdh_freertos.h"

#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

// LVGL
#include "lvgl/lvgl.h"

// Application files
#include "features.h"
#include "app_config.h"

#include "battery.h"
#include "ble_general.h"

#if (FEATURE_DISPLAY)
#include "display.h"
#include "display_brightness.h"
#include "display_boot_up.h"
#include "display_heart_rate.h"
#include "display_home.h"
#include "display_settings.h"
#endif /* FEATURE_DISPLAY */

#include "common.h"
#include "sys_task.h"
#include "side_button.h"
#include "app_sensors.h"

// Drivers
#include "peripherals.h"


#define DEAD_BEEF                       0xDEADBEEF

/** TASK HANDLES **/
TaskHandle_t thSysTask;
TaskHandle_t thBLEMan;
TaskHandle_t thDisplay;
TaskHandle_t thUIupdate;

/** EVENT GROUPS **/
EventGroupHandle_t error_event_group;

/** TIMER HANDLES **/
TimerHandle_t display_timeout_tmr;
TimerHandle_t haptic_timer;
TimerHandle_t button_debounce_timer;

/** QUEUES **/
QueueHandle_t system_queue;
QueueHandle_t settings_queue;
QueueHandle_t display_sensor_info_queue;
QueueHandle_t haptic_queue;
QueueHandle_t ble_action_queue;
QueueHandle_t ble_response_queue;

/** SEMAPHORES **/
SemaphoreHandle_t twi_mutex;
SemaphoreHandle_t spi_mutex;
SemaphoreHandle_t display_drv_mutex;
SemaphoreHandle_t lvgl_mutex;
SemaphoreHandle_t haptic_mutex;
SemaphoreHandle_t button_semphr;

// static lv_disp_buf_t lvgl_disp_buf;
// static lv_color_t lvgl_buf[LV_HOR_RES_MAX * 8];
// static lv_color_t lvgl_buf2[LV_HOR_RES_MAX * 4];
// lv_disp_drv_t lvgl_disp_drv;
// lv_indev_drv_t indev_drv;


/** RTOS INITS **/
void init_queues(void)
{
    // settings_queue      = xQueueCreate(3, sizeof(ChangeSetting_t));
    haptic_queue                = xQueueCreate(3, sizeof(eHaptic_State));
    display_sensor_info_queue   = xQueueCreate(5, sizeof(SensorData_t));
}

void init_semaphores(void)
{
    twi_mutex    = xSemaphoreCreateMutex();
    spi_mutex    = xSemaphoreCreateMutex();
    display_drv_mutex = xSemaphoreCreateMutex();
    lvgl_mutex   = xSemaphoreCreateMutex();
    haptic_mutex = xSemaphoreCreateMutex();
}

void init_timers(void)
{
#if (FEATURE_DISPLAY)
    display_timeout_tmr = xTimerCreate("DisplayTimeout",
                                       pdMS_TO_TICKS(5000),
                                       pdFALSE,
                                       (void *)0,
                                       vDisplayTimeoutCallback);
#endif /* FEATURE_DISPLAY */

    haptic_timer = xTimerCreate("HapticTimer",
                                pdMS_TO_TICKS(100),  // Changes in SysTask
                                pdFALSE,
                                (void *)0,
                                haptic_timer_callback);

    button_debounce_timer = xTimerCreate("button_debounce",
                                         pdMS_TO_TICKS(150),
                                         pdFALSE,
                                         (void *)0,
                                         button_debounce_callback);

}

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 *
 */
void sleep_mode_enter(void)
{
#if (FEATURE_SLEEP)
    ret_code_t err_code;

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
#else
#endif
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

void vApplicationIdleHook( void )
{

}

void vApplicationTickHook(void)
{
    if(xSemaphoreTakeFromISR(lvgl_mutex, 0))
    {
        lv_tick_inc(1);
    }
}

/**
 * @brief Function for initializing the clock.
 */
static void clock_init(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSTAT_SRC_Xtal << CLOCK_LFCLKSTAT_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}


#if(DEBUG_INFO_ENABLED == 1)
static void debug_task(void * arg)
{
    UNUSED_PARAMETER(arg);

    while(1)
    {
//      HeapStats_t xHeapStats;
//      vPortGetHeapStats(&xHeapStats);

        TaskStatus_t current_task_status;
        vTaskGetInfo(thSysTask, &current_task_status, pdTRUE, eReady);
        vTaskGetInfo(thBLEMan, &current_task_status, pdTRUE, eReady);
        vTaskGetInfo(thDisplay, &current_task_status, pdTRUE, eReady);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
#endif


int main(void)
{
    clock_init();

    app_timer_init();

    config_pinout();
    config_peripherals();

#if (FEATURE_DISPLAY)
    // // init display
    // lv_init();
    // lv_disp_buf_init(&lvgl_disp_buf, lvgl_buf, NULL, LV_HOR_RES_MAX * 8);
    // lv_disp_drv_init(&lvgl_disp_drv);
    // lvgl_disp_drv.hor_res = DISPLAY_WIDTH;
    // lvgl_disp_drv.ver_res = DISPLAY_HEIGHT;
    // lvgl_disp_drv.buffer = &lvgl_disp_buf;
    // lvgl_disp_drv.flush_cb = my_flush_cb;
    // lv_disp_drv_register(&lvgl_disp_drv);

    // // init touch input
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = touchscreen_read;
    // lv_indev_drv_register(&indev_drv);
    display_backlight_set(BACKLIGHT_OFF);
#endif /* FEATURE_DISPLAY */

    // Create RTOS components
    init_timers();
    init_semaphores();
    init_queues();

    error_event_group = xEventGroupCreate();

    //
    // Create tasks
    //
    if(pdPASS != xTaskCreate(sys_task,
                             "SysTask",
                             TASK_SYSTASK_STACK_SIZE,
                             NULL,
                             2,
                             &thSysTask))
    {
       APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

#if (FEATURE_DISPLAY)
    if(pdPASS != xTaskCreate(Display_Task,
                             "Display",
                             TASK_DISPLAY_STACK_SIZE,
                             NULL,
                             2,
                             &thDisplay))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

    if(pdPASS != xTaskCreate(UIupdate_Task,
                             "UI Update",
                             TASK_DISPLAY_STACK_SIZE,
                             NULL,
                             4,
                             &thUIupdate))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    vTaskSuspend(thUIupdate);
#endif /* FEATURE_DISPLAY */

#if (DEBUG_INFO_ENABLED == 1)
    if (pdPASS != xTaskCreate(debug_task,
                              "DebugInfo",
                              TASK_BLEGENERAL_STACK_SIZE,
                              NULL,
                              1,
                              NULL))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
#endif /* DEBUG_INFO_ENABLED */

#if (FEATURE_SLEEP)
    // Activate deep sleep mode.
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif /* FEATURE_SLEEP */

#if (FEATURE_BLE)

    // Init BLE
    // power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    db_discovery_init();
    advertising_init();
    peer_manager_init();
    services_init();
    conn_params_init();

    if(pdPASS != xTaskCreate(BLE_Manager_Task,
                            "BLEMan",
                            configMINIMAL_STACK_SIZE,
                            NULL,
                            2,
                            &thBLEMan))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

    bool erase_bonds = false;
    nrf_sdh_freertos_init(advertising_start, &erase_bonds);

#endif /* FEATURE_BLE */

#if (FEATURE_WATCHDOG)
    init_watchdog();
#endif /* FEATURE_WATCHDOG */

    vTaskStartScheduler();

    while(true)
    {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void *malloc( size_t xSize ) {
    for( ;; );
}
