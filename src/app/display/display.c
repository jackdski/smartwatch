//
// Created by jack on 5/24/20.
//

#include "display.h"
#include "sys_task/sys_task.h" // try not to include
#include "lvgl/lvgl.h"
//#include "lvgl/src/lv_themes/"

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

// Drivers
#include "drivers/CST816S.h"

// Display Components
#include "display_boot_up.h"
#include "home.h"
#include "display_common.h"
#include "display_brightness.h"
#include "display_heart_rate.h"
#include "display_settings.h"
#include "display_steps.h"

LV_FONT_DECLARE(jetbrains_mono_bold_20)

extern TimerHandle_t display_timeout_tmr;
extern TimerHandle_t display_lv_handler_tmr;
extern SemaphoreHandle_t lvgl_mutex;
extern SemaphoreHandle_t button_semphr;
extern QueueHandle_t ble_action_queue;
extern QueueHandle_t ble_response_queue;
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
lv_disp_t * disp;
lv_disp_buf_t lvgl_disp_buf;
lv_color_t lvgl_buf[LV_HOR_RES_MAX * 10];
lv_disp_drv_t lvgl_disp_drv;

// TODO: Implement touch input
/*
    lv_indev_drv_t lvgl_indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    bool my_touchpad_read();
 */

// Screens
lv_obj_t * boot_up_scr;
lv_obj_t * home_scr;
lv_obj_t * brightness_scr;
lv_obj_t * settings_scr;
lv_obj_t * steps_scr;
lv_obj_t * heart_rate_scr;


/** Private Functions **/
static void init_display(void)
{
    display_configure();
    display.active = true;
    display_set_rotation(display.rotation_setting);
    display_backlight_set(display.backlight_setting);
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

    // screens
    if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
    {
        boot_up_scr = lv_obj_create(NULL, NULL);    display_boot_up();
        home_scr = lv_obj_create(NULL, NULL);       home_screen();
        brightness_scr = lv_obj_create(NULL, NULL); brightness_screen();
        settings_scr = lv_obj_create(NULL, NULL);   settings_screen();
        settings_scr = lv_obj_create(NULL, NULL);               settings_screen();
        heart_rate_scr = lv_obj_create(NULL, NULL); heart_rate_screen();
        xSemaphoreGive(lvgl_mutex);
    }

    uint32_t button_notif = 0;

    while(1)
    {
        update_brightness();
        update_sleep_status();
        update_charging_status();

        if(xSemaphoreTake(button_semphr, pdMS_TO_TICKS(2)))
        {
            display.button_pressed = true;
            xSemaphoreGive(button_semphr);
        }

        switch(display.display_state)
        {
        case DISPLAY_STATE_INITIALIZATION:
            NRF_LOG_INFO("Display Case Init");
            init_display();
            display.backlight_setting = BACKLIGHT_HIGH;
            update_brightness();

            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE){
                NRF_LOG_INFO("Displaying Boot Up Screen...");
                lv_scr_load(boot_up_scr);
                NRF_LOG_INFO("Load Boot Up Screen...");
                xSemaphoreGive(lvgl_mutex);
                NRF_LOG_INFO("Display Boot Complete")
            }

            uint8_t boot_up_wait = 0;
//            while(dev_not_initialized)
            // wait for SysTask to get init EventBits
//            {
//                vTaskDelay(pdMS_TO_TICKS(1000));
//                boot_up_wait++;
//            }
            vTaskDelay(pdMS_TO_TICKS(5000));
            lv_scr_load(home_scr);
            display_timeout_enable();
            display.display_state = DISPLAY_STATE_RUN;
            break;

        case DISPLAY_STATE_RUN:
            // select graphics based on current info
            if(display.button_pressed == true)
            {
                if(lv_scr_act() == home_scr)
                {
                    display.display_state = DISPLAY_STATE_GO_TO_SLEEP;
                }
                else
                {
                    // TODO: go up one "level"
                    lv_scr_load(home_scr);
//                    home_update_time();
                }
                display.button_pressed = false;
            }

            // update graphics
            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
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