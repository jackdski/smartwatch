/*
 * @author  jdanielski
 * @file    main.c
 */

// nRF includes
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "nordic_common.h"
#include "app_error.h"
#include "nrf_gpiote.h"

// nRF driver files
#include "nrf_sdh_soc.h"
#include "nrf_sdh_freertos.h"
//#include "boards.h"

#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_wdt.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

// LVGL
#include "lvgl/lvgl.h"

// Application files
#include "ble_general.h"
#include "sys_task.h"
#include "battery.h"
#include "display.h"
#include "display_settings.h"
#include "display_heart_rate.h"
#include "home/home.h"
#include "display_brightness.h"
#include "boot_up/display_boot_up.h"
#include "common.h"

// Drivers
#include "drivers/peripherals.h"


#define DEAD_BEEF                       0xDEADBEEF

// Task Handles
extern TaskHandle_t m_logger_thread;                           /**< Definition of Logger thread. */
extern TaskHandle_t thSysTask;
extern TaskHandle_t thBLEMan;
extern TaskHandle_t thDisplay;

// Queues
extern QueueHandle_t settings_queue;
extern QueueHandle_t haptic_queue;
extern QueueHandle_t ble_action_queue;
extern QueueHandle_t ble_response_queue;
extern QueueHandle_t display_info_queue;

// Semaphores/Mutexes
extern SemaphoreHandle_t twi_mutex;
extern SemaphoreHandle_t spi_mutex;
extern SemaphoreHandle_t lvgl_mutex;
extern SemaphoreHandle_t haptic_mutex;
extern SemaphoreHandle_t button_semphr;

// Timers
extern TimerHandle_t display_timeout_tmr;
extern TimerHandle_t display_update_timer;
extern TimerHandle_t haptic_timer;
extern TimerHandle_t button_debounce_timer;

// Event Groups
extern EventGroupHandle_t component_event_group;
extern EventGroupHandle_t error_event_group;

static lv_disp_buf_t lvgl_disp_buf;
static lv_color_t lvgl_buf[LV_HOR_RES_MAX * 4];
lv_disp_drv_t lvgl_disp_drv;
lv_indev_drv_t indev_drv;

//nrf_drv_wdt_channel_id m_channel_id;                    //  watchdog channel

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    // TODO
}

/** GPIO INTERRUPTS **/

void gpio_irq_callback(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    UNUSED_PARAMETER(action);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(pin == PUSH_BUTTON_IN_PIN)
    {
        NRF_LOG_INFO("BUTTON PRESSED");
        bool pin_status = nrf_gpio_pin_read(PUSH_BUTTON_IN_PIN);
        NRF_LOG_INFO("BUTTON STATUS: %d", pin_status);
        xTimerStartFromISR(button_debounce_timer, &xHigherPriorityTaskWoken);
    }
    else if(pin == TP_INT_PIN)
    {
        NRF_LOG_INFO("TS PRESSED");
        system_button_handler();
        xTaskNotifyFromISR(thDisplay, DISPLAY_BUTTON_PRESSED, eSetValueWithOverwrite,&xHigherPriorityTaskWoken);
    }
#if (BMA_IMU_ENABLED == 1)
    else if(pin == BMA421_INT_PIN)
    {
        NRF_LOG_INFO("BMA421 INT");
        system_button_handler();
        xTaskNotifyFromISR(thSysTask, 0xFF, eSetValueWithOverwrite,&xHigherPriorityTaskWoken);
    }
#endif
    else
    {
        NRF_LOG_INFO("UNKNOWN GPIOTE");
    }
}

void button_debounce_callback(TimerHandle_t xTimer)
{
    xTimerStop(button_debounce_timer, 0);
    if(nrf_gpio_pin_read(PUSH_BUTTON_IN_PIN) == false)  // active low
    {
        NRF_LOG_INFO("BUTTON DEBOUNCE PASSED");
        system_button_handler();
    }
    else
    {
        NRF_LOG_INFO("BUTTON FAULT");
    }
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


/**@brief Function for the Timer initialization.
 *
 * @details Initializes RTOS timers
 */
static void rtos_timers_init(void)
{
    display_timeout_tmr = xTimerCreate("DisplayTimeout",
                                       pdMS_TO_TICKS(5000),
                                       pdFALSE,
                                       (void *)0,
                                       vDisplayTimeoutCallback);

    display_update_timer = xTimerCreate("DisplayUpdate",
                                        pdMS_TO_TICKS(500),
                                        pdFALSE,
                                        (void *)0,
                                        vDisplayUpdateCallback);

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

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 *
 */
void sleep_mode_enter(void)
{
    ret_code_t err_code;

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
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
    vTaskResume(m_logger_thread);
}

void vApplicationTickHook(void)
{
    lv_tick_inc(1);
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

/** GPIOTE **/

void config_gpio_interrupts(void)
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
    err_code = nrf_drv_gpiote_in_init(PUSH_BUTTON_IN_PIN, &input_config, gpio_irq_callback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(PUSH_BUTTON_IN_PIN, true);

    // HRS3300 Interrupt
    err_code = nrf_drv_gpiote_in_init(TP_INT_PIN, &input_config, gpio_irq_callback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(TP_INT_PIN, true);

    // BMA421 Interrupt
#if (BMA_IMU_ENABLED == 1)
    err_code = nrf_drv_gpiote_in_init(BMA421_INT_PIN, &input_config, gpio_irq_callback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BMA421_INT_PIN, true);
#endif
}

#if(DEBUG_INFO_ENABLED == 1)
static void debug_task(void * arg)
{
    UNUSED_PARAMETER(arg);

    while(1)
    {
        NRF_LOG_INFO("In debug task");
//      HeapStats_t xHeapStats;
//      vPortGetHeapStats(&xHeapStats);

        uint32_t free_heap = xPortGetFreeHeapSize();
        NRF_LOG_INFO("Heap Remaining (Bytes): %d", free_heap);

        NRF_LOG_INFO("Watermarks:");
        TaskStatus_t current_task_status;
        vTaskGetInfo(thSysTask, &current_task_status, pdTRUE, eReady);
        NRF_LOG_INFO("%s: %d", current_task_status.pcTaskName, current_task_status.usStackHighWaterMark);
        vTaskGetInfo(thBLEMan, &current_task_status, pdTRUE, eReady);
        NRF_LOG_INFO("%s: %d", current_task_status.pcTaskName, current_task_status.usStackHighWaterMark);
        vTaskGetInfo(thDisplay, &current_task_status, pdTRUE, eReady);
        NRF_LOG_INFO("%s: %d", current_task_status.pcTaskName, current_task_status.usStackHighWaterMark);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
#endif

//#if NRF_LOG_ENABLED
/**@brief Thread for handling the logger.
 *
 * @details This thread is responsible for processing log entries if logs are deferred.
 *          Thread flushes all log entries and suspends. It is resumed by idle task hook.
 *
 * @param[in]   arg   Pointer used for passing some arbitrary information (context) from the
 *                    osThreadCreate() call to the thread.
 */
void logger_thread(void * arg)
{
    UNUSED_PARAMETER(arg);

    while (1)
    {
        NRF_LOG_FLUSH();
        vTaskSuspend(NULL); // Suspend myself
    }
}
//#endif //NRF_LOG_ENABLED


int main(void)
{
    clock_init();

    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);

    app_timer_init();

    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("Hello world");

#if (BLINKY_TEST == 1)
    nrf_gpio_cfg_output(DISPLAY_BACKLIGHT_HIGH);

    while(1) {
        uint32_t i;
        for(i = 0; i < 200000; i++);
        NRF_LOG_INFO("Hello world");
        nrf_gpio_pin_toggle(DISPLAY_BACKLIGHT_HIGH);
    }
#else

    // init display
    NRF_LOG_INFO("Init LV");
    lv_init();
    lv_disp_buf_init(&lvgl_disp_buf, lvgl_buf, NULL, LV_HOR_RES_MAX * 4);
    lv_disp_drv_init(&lvgl_disp_drv);
    lvgl_disp_drv.hor_res = DISPLAY_WIDTH;
    lvgl_disp_drv.ver_res = DISPLAY_HEIGHT;
    lvgl_disp_drv.buffer = &lvgl_disp_buf;
    lvgl_disp_drv.flush_cb = my_flush_cb;
    lv_disp_drv_register(&lvgl_disp_drv);

    // init touch input
    NRF_LOG_INFO("Init LV Touch");
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchscreen_read;
    lv_indev_drv_register(&indev_drv);

    config_pinout();
    config_peripherals();
    display_backlight_set(BACKLIGHT_OFF);

    // Create RTOS components
    NRF_LOG_INFO("Init RTOS");
    rtos_timers_init();
    twi_mutex = xSemaphoreCreateMutex();
    spi_mutex = xSemaphoreCreateMutex();
    lvgl_mutex = xSemaphoreCreateMutex();
    haptic_mutex = xSemaphoreCreateMutex();
    button_semphr = xSemaphoreCreateBinary();
    settings_queue = xQueueCreate(3, sizeof(ChangeSetting_t));
    haptic_queue = xQueueCreate(3, sizeof(eHaptic_State));
    ble_action_queue = xQueueCreateCountingSemaphore(5, sizeof(BLEMsg_t));
    ble_response_queue = xQueueCreate(5, sizeof(BLEMsg_t));
    display_info_queue = xQueueCreate(5, sizeof(SensorData_t));
    component_event_group = xEventGroupCreate();
    error_event_group = xEventGroupCreate();

    //
    // Create tasks
    //
//    if(pdPASS != xTaskCreate(sys_task,
//                     "SysTask",
//                             TASK_SYSTASK_STACK_SIZE,
//                             NULL,
//                             3,
//                             &thSysTask))
//    {
//        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
//    }

    if(pdPASS != xTaskCreate(Display_Task,
                     "Display",
                             256,
                             NULL,
                             2,
                             &thDisplay))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

    // Start execution.
    if (pdPASS != xTaskCreate(logger_thread,
                              "LOGGER",
                              256,
                              NULL,
                              1,
                              &m_logger_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

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

    // Activate deep sleep mode.
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
//    nrf_drv_wdt_channel_feed(m_channel_id);

    NRF_LOG_INFO("BLE Init");
    ble_stack_init();

    // Init BLE
    NRF_LOG_INFO("BLE Init 2");
    power_management_init();
    gap_params_init();
    gatt_init();
    db_discovery_init();
    services_init();
    advertising_init();
    peer_manager_init();
    conn_params_init();

    NRF_LOG_INFO("BLE Init 3");
    nrf_sdh_freertos_init(advertising_start, NULL);

//    NRF_LOG_INFO("BLE Init 4");
//    if(pdPASS != xTaskCreate(BLE_Manager_Task,
//                             "BLEMan",
//                             configMINIMAL_STACK_SIZE,
//                             NULL,
//                             2,
//                             &thBLEMan))
//    {
//        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
//    }

#if (WATCHDOG_ENABLED == 1)
    //Configure WDT. TODO: move to separate file
    ret_code_t err_code;
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
    NRF_LOG_INFO("Watchdog Configured...");
#endif

    config_gpio_interrupts();

    uint32_t free_heap = xPortGetFreeHeapSize();
    NRF_LOG_INFO("Heap Remaining (Bytes): %d", free_heap);

    init_display();
    NRF_LOG_INFO("Start Scheduler...");
    vTaskStartScheduler();

    while(true)
    {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
#endif
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    NRF_LOG_INFO("OVERFLOW TASK: %s", pcTaskName);
    ( void ) pcTaskName;
    ( void ) pxTask;
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void *malloc( size_t xSize ) {
    for( ;; );
}
