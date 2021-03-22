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
#include "touchscreen/CST816S.h"
#include "app_sensors.h"
#include "battery.h"
#include "settings.h"
#include "display_drv.h"
#include "components/time/time.h"

// Display Components
#include "boot_up/display_boot_up.h"
#include "home/home.h"
#include "display_common.h"
#include "display_brightness.h"
#include "display_heart_rate.h"
#include "display_settings.h"
#include "display_steps.h"

//LV_FONT_DECLARE(jetbrains_mono_bold_20)

extern TimerHandle_t display_timeout_tmr;
extern TimerHandle_t display_update_timer;
extern SemaphoreHandle_t lvgl_mutex;
extern SemaphoreHandle_t button_semphr;
extern QueueHandle_t ble_action_queue;
extern QueueHandle_t ble_response_queue;
extern QueueHandle_t display_info_queue;
extern QueueHandle_t settings_queue;
extern EventGroupHandle_t component_event_group;

static Display_Control_t display = {
    .initialized        = false,
    .active             = true,
    .always_on          = false,
    .debug              = true,
    .charging           = false,
    .button_pressed     = false,
    .touch_active       = false,
    .battery_status     = BATTERY_EMPTY,
    .state              = DISPLAY_STATE_INITIALIZATION,
    .backlight_setting  = BACKLIGHT_HIGH,
    .rotation_setting   = DISPLAY_ROTATION_0,  // TODO: retrieve from non-volatile memory
    .soc                = 0,
    .heart_rate         = 0
};

static SensorData_t display_sensor_data = {0};


/** Private Functions **/
void init_display(void)
{
    display_configure();
    display.active = true;
//    display_set_rotation(display.rotation_setting);
}

static void update_sleep_status(void)
{
    if(display.state == DISPLAY_STATE_SLEEP)
    {
        xEventGroupSetBits(component_event_group, COMPONENT_DISPLAY);
        vTaskSuspend(xTaskGetCurrentTaskHandle());
    }
    else
    {
        xEventGroupClearBits(component_event_group, COMPONENT_DISPLAY);
    }
}

/**
 *  Determines which information about battery and charging to display
 */
static void determine_battery_info(Display_Control_t * d, SensorData_t sensor_data)
{
    // Update charging info
    if(sensor_data.battery_events & BATTERY_CHARGING)
    {
        d->charging = true;
    }
    else
    {
        d->charging = false;
    }

    // Update battery status info
    if(sensor_data.battery_events & BATTERY_LOW_POWER)
    {
        d->battery_status = BATTERY_EMPTY;
    }
    else
    {
        uint32_t battery_status_mask = (BATTERY_STATUS_FULL | BATTERY_STATUS_HIGH | BATTERY_STATUS_MEDIUM | BATTERY_STATUS_LOW);
        uint32_t status = (sensor_data.battery_events & battery_status_mask);

        switch(status)
        {
        case BATTERY_STATUS_FULL:
            d->battery_status = BATTERY_FULL;
            break;
        case BATTERY_STATUS_HIGH:
            d->battery_status = BATTERY_ALMOST_FULL;
            break;
        case BATTERY_STATUS_MEDIUM:
            d->battery_status = BATTERY_HALF_FULL;
            break;
        case BATTERY_STATUS_LOW:
        default:
            d->battery_status = BATTERY_LOW;
            break;
        }
    }
}

static void update_brightness(void)
{
    display_backlight_set(display.backlight_setting);
}

static void display_handle_button(void)
{
    Display_Screens_E screen;
    switch(display.state)
    {
        case DISPLAY_STATE_RUN:
            if(display.screen == DISPLAY_SCREEN_HOME)
            {
                display.state = DISPLAY_STATE_GO_TO_SLEEP;
            }
            else
            {
                screen = display_get_parent_screen(display.screen);
                display_change_screen(screen);
            }
            break;
        case DISPLAY_STATE_GO_TO_SLEEP:
            display.state = DISPLAY_STATE_RUN;
            screen = display_get_parent_screen(display.screen);
            display_change_screen(screen);
            break;
        case DISPLAY_STATE_INITIALIZATION:
        case DISPLAY_STATE_SLEEP:
        case DISPLAY_STATE_ERROR:
        default:
            break;
    }
    display_timeout_refresh();
}


/** Public Functions **/

void Display_Task(void * arg)
{
    UNUSED_PARAMETER(arg);

    NRF_LOG_INFO("Init Display Task");
    display_timeout_disable();

    while(1)
    {
        switch(display.state)
        {
        case DISPLAY_STATE_INITIALIZATION:
            NRF_LOG_INFO("Display Case Init");
            display.backlight_setting = BACKLIGHT_HIGH;
            update_brightness();
//            init_display();

//            // init touch input
//            NRF_LOG_INFO("Init LV Touch");
//            lv_indev_drv_init(&indev_drv);
//            indev_drv.type = LV_INDEV_TYPE_POINTER;
//            indev_drv.read_cb = touchscreen_read;
//            lv_indev_drv_register(&indev_drv);

            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
            {
                NRF_LOG_INFO("Displaying Boot Up Screen...");
                display_boot_up();
                display.screen = DISPLAY_SCREEN_INITIALIZATION;
                NRF_LOG_INFO("Loaded Boot Up Screen...");
                xSemaphoreGive(lvgl_mutex);
                NRF_LOG_INFO("Display Boot Complete")
            }

            display_timeout_enable();
            display_change_screen(DISPLAY_SCREEN_HOME);
            display.state = DISPLAY_STATE_RUN;
            break;

        case DISPLAY_STATE_RUN:
            // select graphics based on current info
            if(display.button_pressed)
            {
                display_handle_button();
                display.button_pressed = false;
            }

            // update graphics
            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
            {
                lv_task_handler();
                xSemaphoreGive(lvgl_mutex);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
            break;

        case DISPLAY_STATE_GO_TO_SLEEP:
            display.backlight_setting = BACKLIGHT_OFF;
            display_backlight_set(display.backlight_setting);
            display_off();
            display_go_to_sleep();
            xTimerStop(display_update_timer, pdMS_TO_TICKS(5));
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
            break;
        }
    }
}

/**   Timer Calbacks   **/
void vDisplayTimeoutCallback(TimerHandle_t xTimer)
{
    UNUSED_PARAMETER(xTimer);
    if(display.always_on == false)
    {
        display_timeout_disable();
        display.state = DISPLAY_STATE_GO_TO_SLEEP;
    }
}

void vDisplayUpdateCallback(TimerHandle_t xTimer)
{
    UNUSED_PARAMETER(xTimer);
    NRF_LOG_INFO("Display Task Update");
    update_brightness();
    update_sleep_status();
    if(xQueueReceive(display_info_queue, &display_sensor_data, 5))
    {
        determine_battery_info(&display, display_sensor_data);
    }

    // touchscreen
    uint32_t touchscreen_pressed = false;
    if(xTaskNotifyWait(0, 0, &touchscreen_pressed, 0))
    {
        CST816S_read_touch();
    }
    display.touch_active = CST816S_get_touch_active();

    // button
    if(xSemaphoreTake(button_semphr, pdMS_TO_TICKS(0)))
    {
        display.button_pressed = true;
        xSemaphoreGive(button_semphr);
    }

    // timeout timer refresh
    if(display.touch_active || display.button_pressed)
    {
        display_timeout_refresh();
    }

//    if(get_minute() != minute_update_cmp)
//    {
//        minute_update_cmp = get_minute();
//    }
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


/**   LVGL Callbacks   */

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
#define USE_UPDATED_FLASH_CB    0
#if (USE_UPDATED_FLASH_CB == 1)
    // TODO: try out updating 100 pixels at a time instead of 1 pixel to avoid taking/releasing semaphore all the time
    int16_t width = (area->x2 - area->x1);
    int16_t height = (area->y2 - area->y1);
    uint32_t area_size = width * height;
    display_set_address_window(area->x1, area->x2, area->y1, area->y2);
//    display_write_data(color_p, width * height * 2);

    while(area_size > 0)
    {
        uint8_t size = area_size ? (area_size < 100): 100;
        uint8_t rgb[size * 2];
        uint8_t i;
        for(i = 0; i < size; i++)
        {
            rgb[i] = ((color_p->full >> 8) & 0xFF), (color_p->full & 0xFF);
            color_p++;
        }
        display_write_data(rgb, sizeof(rgb));
        area_size -= size;
    }
#else
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++)
    {
        for(x = area->x1; x <= area->x2; x++)
        {
            display_draw_pixel(x, y, color_p->full);
            color_p++;
        }
    }
#endif

    lv_disp_flush_ready(disp_drv);
}

/** Touchscreen **/
bool touchscreen_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint16_t x_last = 0;
    static uint16_t y_last = 0;
    uint16_t x = 0;
    uint16_t y = 0;

    if(CST816S_get_touch_active())
    {
        CST816S_get_xy(&x_last, &y_last);
        x_last = x;
        y_last = y;
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = x;
    data->point.y = y;

    return false;
}


/**   Display Library Functions   */

void display_change_screen(Display_Screens_E screen)
{
    if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(10)))
    {
        display.screen = screen;
        switch(screen) {
        case DISPLAY_SCREEN_INITIALIZATION:     display_boot_up(); break;
        case DISPLAY_SCREEN_SETTINGS:           display_settings_screen(); break;
        case DISPLAY_SCREEN_BRIGHTNESS:         brightness_screen(); break;
        case DISPLAY_SCREEN_STEPS:              steps_screen(); break;
        case DISPLAY_SCREEN_HEART_RATE:         heart_rate_screen(); break;
        case DISPLAY_SCREEN_TEXT_MESSAGE:       /*text_message_screen();*/ break;
        case DISPLAY_SCREEN_PHONE_NOTIFICATION: /*phone_notification_screen();*/ break;
        case DISPLAY_SCREEN_HOME:
        default:
            home_screen();
            break;
        }
        xSemaphoreGive(lvgl_mutex);
    }
}

Display_Screens_E display_get_parent_screen(Display_Screens_E screen)
{
    switch(screen) {
        case DISPLAY_SCREEN_INITIALIZATION:
            return DISPLAY_SCREEN_INITIALIZATION;
        case DISPLAY_SCREEN_HOME:
        case DISPLAY_SCREEN_SETTINGS:
        case DISPLAY_SCREEN_BRIGHTNESS:
        case DISPLAY_SCREEN_STEPS:
        case DISPLAY_SCREEN_HEART_RATE:
        case DISPLAY_SCREEN_TEXT_MESSAGE:
        case DISPLAY_SCREEN_PHONE_NOTIFICATION:
        default:
            return DISPLAY_SCREEN_HOME;
    }
}

bool display_get_charging_status(void)
{
    return display.charging;
}

eDisplayBatteryStatus display_get_battery_status(void)
{
    return display.battery_status;
}

// Shared Data
void display_setting_changed(eSetting setting)
{
    ChangeSetting_t new_setting;
    new_setting.setting = setting;
//    xQueueSend(settings_queue, &new_setting, 0);
}


// Test Functions
void display_brightness_test(void)
{
    eBacklightSetting backlights[] = {BACKLIGHT_LOW, BACKLIGHT_MID, BACKLIGHT_HIGH, BACKLIGHT_MID, BACKLIGHT_LOW, BACKLIGHT_OFF, BACKLIGHT_HIGH};
    uint8_t backlight;
    for(backlight = 0; backlight < 7; backlight++)
    {
        NRF_LOG_INFO("Backlight: %d", backlights[backlight]);
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
        NRF_LOG_INFO("Displaying: %d", colors[color]);
    }
}

