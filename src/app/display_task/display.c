//
// Created by jack on 5/24/20.
//

#include "display.h"
#include "lvgl/lvgl.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
#include "queue.h"

// Drivers and Components
#include "CST816S.h"
#include "app_sensors.h"
#include "battery.h"
#include "settings.h"
#include "display_drv.h"
#include "components/time/time.h"

// Display Components
#include "drv_touchscreen.h"
#include "display_alerts.h"
#include "display_boot_up.h"
#include "display_home.h"
#include "display_common.h"
#include "display_brightness.h"
#include "display_heart_rate.h"
#include "display_settings.h"
#include "display_steps.h"


#define BOOT_UP_SCREEN_TIMEOUT_MS          3000U       // [ms]
#define DISPLAY_BUF_SIZE    (LV_HOR_RES_MAX * 8)   

//LV_FONT_DECLARE(jetbrains_mono_bold_20)


extern TimerHandle_t        display_timeout_tmr;
extern TimerHandle_t        display_alert_tmr;
extern SemaphoreHandle_t    lvgl_mutex;
extern SemaphoreHandle_t    button_semphr;
extern QueueHandle_t        ble_action_queue;
extern QueueHandle_t        ble_response_queue;
extern QueueHandle_t        display_sensor_info_queue;
extern QueueHandle_t        settings_queue;


lv_disp_buf_t   lvgl_disp_buf;
lv_color_t      lvgl_buf[DISPLAY_BUF_SIZE];
lv_disp_drv_t   lvgl_disp_drv;
lv_indev_drv_t  indev_drv;



static Display_Control_t display =
{
    .initialized        = false,
    .active             = true,
    .always_on          = false,
    .debug              = true,
    .button_pressed     = false,
    .touch_active       = false,
    .state              = DISPLAY_STATE_INITIALIZATION,
    .backlight_setting  = BACKLIGHT_HIGH,
    .rotation_setting   = DISPLAY_ROTATION_0  // TODO: retrieve from non-volatile memory
};

static DisplayData_t display_info =
{
    .charging       = false,
    .battery_status = BATTERY_EMPTY,
    .soc            = 0,
    .heart_rate     = 0
};


/** PRIVATE FUNCTIONS  **/

static void init_display(void)
{
    display_configure();
    display.active = true;
//    display_set_rotation(display.rotation_setting);
}

static void update_sleep_status(void)
{
    if(display.state == DISPLAY_STATE_SLEEP)
    {
        vTaskSuspend(xTaskGetCurrentTaskHandle());
    }
}

/**
 *  Determines which information about battery and charging to display
 */
static void update_battery_info(DisplayData_t * d, SensorData_t sensor_data)
{
    // Update charging info
    if(sensor_data.battery.charging_state == BATTERY_CHARGING_ACTIVE)
    {
        d->charging = true;
    }
    else
    {
        d->charging = false;
    }

    // Update battery status info
    switch(sensor_data.battery.level)
    {
    case BATTERY_LEVEL_FULL:
        d->battery_status = BATTERY_FULL;
        break;
    case BATTERY_LEVEL_HIGH:
        d->battery_status = BATTERY_ALMOST_FULL;
        break;
    case BATTERY_LEVEL_MEDIUM:
        d->battery_status = BATTERY_HALF_FULL;
        break;
    case BATTERY_LEVEL_LOW:
        if(sensor_data.battery.soc > 10)
        {
            d->battery_status = BATTERY_LOW;
        }
        else
        {
            d->battery_status = BATTERY_EMPTY;
        }
    default:
        d->battery_status = BATTERY_LOW;
        break;
    }
}

// static void update_heart_rate_info(DisplayData_t * display_data)
// {
//     return;
// }

static void update_brightness(eBacklightSetting backlight_setting)
{
    display.backlight_setting = backlight_setting;
    display_backlight_set(display.backlight_setting);
}

static void display_handle_button(void)
{
    const Display_States_E state = display.state;
    DisplayScreens_E parent_screen = display_get_parent_screen(display.screen);

    switch(state)
    {
        case DISPLAY_STATE_RUN:
            if(display.screen == DISPLAY_SCREEN_HOME)
            {
                display.state = DISPLAY_STATE_GO_TO_SLEEP;
            }
            else
            {
                display.screen = display_change_screen(parent_screen);
            }
            break;
        
        case DISPLAY_STATE_GO_TO_SLEEP:
            display.state = DISPLAY_STATE_RUN;
            display.screen = display_change_screen(parent_screen);
            break;

        case DISPLAY_STATE_INITIALIZATION:
        case DISPLAY_STATE_SLEEP:
        case DISPLAY_STATE_ERROR:
        default:
            break;
    }
}

static void vDisplayUpdate(void)
{
    // system status updates
    update_sleep_status();
    bool ui_active = (display.state != DISPLAY_STATE_GO_TO_SLEEP);

    SensorData_t display_sensor_data;
    if(xQueueReceive(display_sensor_info_queue, &display_sensor_data, 0))
    {
        update_battery_info(&display_info, display_sensor_data);
    }

    // touchscreen
    uint32_t touchscreen_pressed = false;
    if(xTaskNotifyWait(0, 0, &touchscreen_pressed, 0))
    {
        CST816S_read_touch();
    }
    display.touch_active = CST816S_get_touch_active();

    bool alert_active = display_check_for_alerts();
    display.button_pressed = (xSemaphoreTake(button_semphr, 0) == pdTRUE);

    if(alert_active)
    {
        set_return_screen(display.screen);
        display_change_screen(DISPLAY_SCREEN_ALERT);
        display_alert_timer_enable();
    }
    else
    {
        // timeout timer refresh
        if(ui_active || display.touch_active || display.button_pressed)
        {
            display_timeout_refresh();
            display.button_pressed = false;
        }
        else
        {
            display.state = DISPLAY_STATE_GO_TO_SLEEP;
        }

        if(display.button_pressed)
        {
            display_handle_button();
        }
    }
}


/** Public Functions **/
void UIupdate_Task(void * arg)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(10);

    while(1)
    {
        // update graphics
        if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
        {
            lv_task_handler();
            xSemaphoreGive(lvgl_mutex);
        }
        vTaskDelay(xFrequency);
    }    
}

void Display_Task(void * arg)
{
    UNUSED_PARAMETER(arg);

    display_timeout_disable();

    // init display
    lv_init();
    lv_disp_buf_init(&lvgl_disp_buf, lvgl_buf, NULL, DISPLAY_BUF_SIZE);
    lv_disp_drv_init(&lvgl_disp_drv);
    lvgl_disp_drv.hor_res = DISPLAY_WIDTH;
    lvgl_disp_drv.ver_res = DISPLAY_HEIGHT;
    lvgl_disp_drv.buffer = &lvgl_disp_buf;
    lvgl_disp_drv.flush_cb = my_flush_cb;
    lv_disp_drv_register(&lvgl_disp_drv);

    // init touch input
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchscreen_read;
    lv_indev_drv_register(&indev_drv);

    init_display();

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20);

    while(1)
    {
        switch(display.state)
        {
        case DISPLAY_STATE_INITIALIZATION:
            
            // init touch input
            lv_indev_drv_init(&indev_drv);
            indev_drv.type = LV_INDEV_TYPE_POINTER;
            indev_drv.read_cb = touchscreen_read;
            lv_indev_drv_register(&indev_drv);

            // show boot up screen for brief period
            display.screen = display_change_screen(DISPLAY_SCREEN_INITIALIZATION);
            update_brightness(BACKLIGHT_HIGH);

            display.screen = display_change_screen(DISPLAY_SCREEN_HOME);
            // vTaskResume(thUIupdate);

            display_timeout_enable();
            display.state = DISPLAY_STATE_RUN;
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
            break;

        case DISPLAY_STATE_RUN:
            vDisplayUpdate();

            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
            {
                lv_task_handler();
                xSemaphoreGive(lvgl_mutex);
            }
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
            break;

        case DISPLAY_STATE_GO_TO_SLEEP:
            display_backlight_set(BACKLIGHT_OFF);
            display_off();
            display_go_to_sleep();
            display.active = false;
            display.state = DISPLAY_STATE_SLEEP;
            vTaskSuspend(xTaskGetCurrentTaskHandle());
            break;

        case DISPLAY_STATE_SLEEP:
            if(display.initialized)
            {
                display.state = DISPLAY_STATE_RUN;
                display_timeout_enable();
            }
            break;

        case DISPLAY_STATE_ERROR:
        default:
            NRF_BREAKPOINT;
            break;
        }
    }
}

/**   Timer Calbacks   **/
void vDisplayTimeoutCallback(TimerHandle_t xTimer)
{
    UNUSED_PARAMETER(xTimer);

    display_timeout_disable();
    display.state = DISPLAY_STATE_GO_TO_SLEEP;
}

void display_timeout_refresh(void)
{
    if(xTimerReset(display_timeout_tmr, pdMS_TO_TICKS(100)) == pdPASS)
    {
        display.active = true;
        display.state = DISPLAY_STATE_RUN;
    }
}

void display_timeout_enable(void)
{
    xTimerStart(display_timeout_tmr, 0);
}

void display_timeout_disable(void)
{
    xTimerStop(display_timeout_tmr, 0);
}

void display_alert_timer_enable(void)
{
    xTimerStart(display_alert_tmr, 0);
}

void display_alert_timer_disable(void)
{
    xTimerStop(display_alert_tmr, 0);
}


/**   LVGL   */
#define USE_UPDATED_FLUSH_CB 0

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
#if USE_UPDATED_FLUSH_CB
    // TODO: try out updating 100 pixels at a time instead of 1 pixel to avoid taking/releasing semaphore all the time
    int16_t width = (area->x2 - area->x1) + 1;
    int16_t height = (area->y2 - area->y1) + 1;
    uint32_t area_size = width * height;
    display_set_address_window(area->x1, area->x2, area->y1, area->y2);
//    display_write_data(color_p, width * height * 2);

    // uint8_t size = area_size ? (area_size < 100): 100;
    uint8_t buf[area_size * 2];
    uint8_t i;
    for(i = 0; i < (area_size * 2); i += 2)
    {
        buf[i] = (color_p->full & 0xFF00)>> 8;
        buf[i+1] =  (color_p->full & 0xFF);
        color_p++;
    }
    display_write_data(buf, sizeof(buf));
#else
    int32_t x, y;
    // display_set_address_window(area->x1, area->x2 - 1, area->y1, area->y2 - 1);
    display_set_address_window(area->x1, area->x2, area->y1, area->y2);
    for(y = area->y1; y <= area->y2; y++)
    {
        for(x = area->x1; x <= area->x2; x++)
        {
            // display_draw_pixel(x, y, color_p->full);
            uint8_t rgb[] = {(color_p->full >> 8), (color_p->full & 0xFF)};
            display_write_data(rgb, sizeof(rgb));
            color_p++;
        }
    }
#endif

    lv_disp_flush_ready(disp_drv);
}


/**   Display Library Functions   */

bool display_get_charging_status(void)
{
    return display_info.charging;
}

DisplayBatteryStatus_E display_get_battery_status(void)
{
    return display_info.battery_status;
}

DisplayScreens_E display_get_current_screen(void)
{
    return display.screen;
}

void display_setting_changed(eSetting setting)
{
    UNUSED_PARAMETER(setting);
//     ChangeSetting_t new_setting;
//     new_setting.setting = setting;
// //    xQueueSend(settings_queue, &new_setting, 0);
}

#if 0
// Display Test Functions
void display_brightness_test(void)
{
    eBacklightSetting backlights[] = {BACKLIGHT_LOW, BACKLIGHT_MID, BACKLIGHT_HIGH, BACKLIGHT_MID, BACKLIGHT_LOW, BACKLIGHT_OFF, BACKLIGHT_HIGH};
    uint8_t backlight;
    for(backlight = 0; backlight < 7; backlight++)
    {
        display_backlight_set(backlights[backlight]);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void display_color_fill_test(void)
{
    uint8_t colors[] = {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
                        0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0xFF};
    uint8_t color;
    for(color = 0; color < sizeof(colors); color++) {
        display_draw_fill(colors[color]);
    }
}
#endif

