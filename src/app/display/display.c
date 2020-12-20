//
// Created by jack on 5/24/20.
//

#include "display.h"
#include "sys_task.h" // try not to include
#include "lvgl/lvgl.h"

// nRF Logging includes
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

// Drivers and Components
#include "CST816S.h"
#include "battery.h"
#include "settings.h"
#include "display_drv.h"
#include "components/time/time.h"

// Display Components
#include "display_boot_up.h"
#include "home.h"
#include "display_common.h"
#include "display_brightness.h"
#include "display_heart_rate.h"
#include "display_settings.h"
#include "display_steps.h"

//LV_FONT_DECLARE(jetbrains_mono_bold_20)

extern TimerHandle_t display_timeout_tmr;
extern TimerHandle_t display_lv_handler_tmr;
extern SemaphoreHandle_t lvgl_mutex;
extern SemaphoreHandle_t button_semphr;
extern QueueHandle_t ble_action_queue;
extern QueueHandle_t ble_response_queue;
extern QueueHandle_t settings_queue;
extern EventGroupHandle_t component_event_group;
extern EventGroupHandle_t charging_event_group;


typedef struct {
  bool                initialized:1;
  bool                active:1;
  bool                always_on:1;
  bool                debug:1;
  bool                charging:1;
  bool                button_pressed:1;
  Display_States_E    display_state;
  Display_Screens_E   screen;
  eBacklightSetting   backlight_setting;
  eDisplayRotation    rotation_setting;
} Display_Control_t;

static Display_Control_t display = {
    .initialized = false,
    .active = true,
    .always_on = true,
    .debug = true,
    .charging = false,
    .button_pressed = false,
    .display_state = DISPLAY_STATE_INITIALIZATION,
    .backlight_setting = BACKLIGHT_HIGH,
    .rotation_setting = DISPLAY_ROTATION_0  // TODO: retrieve from non-volatile memory
};


// LVGL
//lv_disp_t * disp;
//lv_disp_drv_t lvgl_disp_drv;
//lv_indev_drv_t indev_drv;

// Screens
lv_obj_t * main_scr;


/** Private Functions **/
void init_display(void)
{
    display_configure();
    display.active = true;
//    display_set_rotation(display.rotation_setting);
}

static void update_sleep_status(void)
{
    if(display.display_state == DISPLAY_STATE_SLEEP)
    {
        xEventGroupSetBits(component_event_group, COMPONENT_DISPLAY);
        vTaskSuspend(xTaskGetCurrentTaskHandle());
    }
    else
    {
        xEventGroupClearBits(component_event_group, COMPONENT_DISPLAY);
    }
}

static void update_charging_status(void)
{
    if(xEventGroupGetBits(charging_event_group) & BATTERY_CHARGING)
    {
        display.charging = true;
        if(xEventGroupGetBits(charging_event_group) & BATTERY_CHARGING_STARTED)
        {
            // charging is started - update screen
            xEventGroupClearBits(charging_event_group, BATTERY_CHARGING_STARTED);
        }
    }
    else
    {
        display.charging = false;
    }
}

static void update_brightness(void)
{
    display_backlight_set(display.backlight_setting);
}

static void display_handle_button(uint8_t button_presses)
{
    // TODO
}


/** Public Functions **/

void Display_Task(void * arg)
{
    UNUSED_PARAMETER(arg);

    NRF_LOG_INFO("Init Display Task");
    display_timeout_disable();

    uint32_t button_notif = 0;
    uint32_t update_counter = 0; // xTaskGetTickCount();
    bool update_time = 0;
    static uint8_t minute_update_cmp = 0;
    static lv_obj_t * text;
    static lv_style_t label_style;

    while(1) {
//        if(xTaskGetTickCount() - update_counter >= 1000)
//        {
//            NRF_LOG_INFO("Display Task Update");
//            update_brightness();
//            update_sleep_status();
//            update_charging_status();
//            if(get_minute() != minute_update_cmp)
//            {
//                update_time = true;
//                minute_update_cmp = get_minute();
//            }
//            update_counter = xTaskGetTickCount();
//        }

//        if(xSemaphoreTake(button_semphr, pdMS_TO_TICKS(2)))
//        {
//            display.button_pressed = true;
//            xSemaphoreGive(button_semphr);
//        }

        switch(display.display_state)
        {
        case DISPLAY_STATE_INITIALIZATION:
            NRF_LOG_INFO("Display Case Init");
            display.backlight_setting = BACKLIGHT_HIGH;
            update_brightness();
            init_display();

            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE){
                NRF_LOG_INFO("Displaying Boot Up Screen...");
                display_boot_up();
                display.screen = DISPLAY_SCREEN_INITIALIZATION;
                NRF_LOG_INFO("Loaded Boot Up Screen...");
                xSemaphoreGive(lvgl_mutex);
                NRF_LOG_INFO("Display Boot Complete")
            }

            TickType_t start = xTaskGetTickCount();
            while(xTaskGetTickCount() - start < 5000)
            {
                if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
                    lv_task_handler();
                    xSemaphoreGive(lvgl_mutex);
                }
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            display_timeout_enable();
            home_screen();
            display.screen = DISPLAY_SCREEN_HOME;
            display.display_state = DISPLAY_STATE_RUN;
            break;

        case DISPLAY_STATE_RUN:
            display_show_screen();

            // select graphics based on current info
            if(display.button_pressed == true)
            {
                if(display.screen == DISPLAY_SCREEN_HOME)
                {
                    display.display_state = DISPLAY_STATE_GO_TO_SLEEP;
                }
                else
                {
                    // TODO: go up one "level"
                }
                display.button_pressed = false;
            }

            // update graphics
            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
                if(update_time)
                {
                    home_screen();
                }
                lv_task_handler();
                xSemaphoreGive(lvgl_mutex);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
            break;

        case DISPLAY_STATE_GO_TO_SLEEP:
            display.backlight_setting = BACKLIGHT_OFF;
            display_backlight_set(display.backlight_setting);
            display_off();
            display.active = false;
            display.display_state = DISPLAY_STATE_SLEEP;
            vTaskSuspend(xTaskGetCurrentTaskHandle());
            break;

        case DISPLAY_STATE_SLEEP:
            if(display.initialized == true)
            {
                display.display_state = DISPLAY_STATE_RUN;
            }
            break;

        case DISPLAY_STATE_ERROR:
        default:
            break;
        }
    }
}

void vDisplayTimeoutCallback(TimerHandle_t xTimer)
{
    UNUSED_PARAMETER(xTimer);
    if(display.always_on == false) {
        display_timeout_disable();
        display.backlight_setting = BACKLIGHT_OFF;
        update_brightness();
        display.active = false;
        display.display_state = DISPLAY_STATE_GO_TO_SLEEP;
    }
}

void display_timeout_refresh(void)
{
    if(xTimerReset(display_timeout_tmr, pdMS_TO_TICKS(100)) == pdPASS) {
        display.active = true;
        display.display_state = DISPLAY_STATE_RUN;
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

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
//    int16_t width, height = 0;
//    width = (area->x2 - area->x1);
//    height = (area->y2 - area->y1);
//    display_set_address_window(area->x1, area->x2, area->y1, area->y2);
//    display_write_data((uint8_t *) color_p, width * height * 2);

    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++)
    {
        for(x = area->x1; x <= area->x2; x++)
        {
            display_draw_pixel(x, y, color_p->full);
            color_p++;
        }
    }

    lv_disp_flush_ready(disp_drv);
}

void display_set_screen(Display_Screens_E screen)
{
    display.screen = screen;
}

void display_show_screen(void)
{
    const Display_Screens_E screen = display.screen;
    switch(screen) {
        case DISPLAY_SCREEN_INITIALIZATION:     display_boot_up(); break;
        case DISPLAY_SCREEN_HOME:               home_screen(); break;
        case DISPLAY_SCREEN_SETTINGS:           display_settings_screen(); break;
        case DISPLAY_SCREEN_BRIGHTNESS:         /* display_brightness_screen(); */ break;
        case DISPLAY_SCREEN_STEPS:              /* display_steps_screen(); */ break;
        case DISPLAY_SCREEN_HEART_RATE:         /* display_heart_rate_screen(); */ break;
        case DISPLAY_SCREEN_TEXT_MESSAGE:       /* display_text_message_screen(); */ break;
        case DISPLAY_SCREEN_PHONE_NOTIFICATION: /* display_phone_notification_screen(); */ break;
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
    // TODO
    return BATTERY_FULL;
}

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

/** Touchscreen **/
bool read_touchscreen(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint16_t x_last;
    static uint16_t y_last;
    uint16_t x;
    uint16_t y;

    if(CST816S_read_touch(&x, &y))
    {
        x_last = x;
        y_last = y;
    }
    else
    {
        x = x_last;
        y = y_last;
    }

    data->point.x = x;
    data->point.y = y;

    if(CST816S_isTouchActive())
    {
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    return false;
}

// Shared Data
void display_setting_changed(eSetting setting)
{
    ChangeSetting_t new_setting;
    new_setting.setting = setting;
    xQueueSend(settings_queue, &new_setting, 0);
}
