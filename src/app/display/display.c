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



static Display_Control_t display_data =
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
    display_data.active = true;
//    display_set_rotation(display_data.rotation_setting);
}

static void display_updateSleepStatus(void)
{
    if (display_data.state == DISPLAY_STATE_SLEEP)
    {
        vTaskSuspend(xTaskGetCurrentTaskHandle());
    }
}

/**
 *  Determines which information about battery and charging to display
 */
static void display_updateBatteryInfo(DisplayData_t * d, SensorData_t sensor_data)
{
    // Update charging info
    if (sensor_data.battery.charging_state == BATTERY_CHARGING_ACTIVE)
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
        if (sensor_data.battery.soc > 10)
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

static void disaplay_updateBrightness(eBacklightSetting backlight_setting)
{
    display_data.backlight_setting = backlight_setting;
    display_backlight_set(display_data.backlight_setting);
}

static void display_buttonHandler(void)
{
    const Display_States_E state = display_data.state;
    const DisplayScreens_E currentScreen = display_data.screen;
    display_data.screen = display_get_parent_screen(currentScreen);

    switch(state)
    {
        case DISPLAY_STATE_RUN:
            if (currentScreen == DISPLAY_SCREEN_HOME)
            {
                display_data.state = DISPLAY_STATE_GO_TO_SLEEP;
            }
            else
            {
                display_change_screen(display_data.screen);
            }
            break;

        case DISPLAY_STATE_GO_TO_SLEEP:
            display_data.state = DISPLAY_STATE_RUN;
            display_change_screen(display_data.screen);
            break;

        case DISPLAY_STATE_INITIALIZATION:
        case DISPLAY_STATE_SLEEP:
        case DISPLAY_STATE_ERROR:
        default:
            break;
    }
}

static void display_timeoutRefresh(void)
{
    if (xTimerReset(display_timeout_tmr, pdMS_TO_TICKS(100)) == pdPASS)
    {
        display_data.active = true;
        display_data.state = DISPLAY_STATE_RUN;
    }
}

static void display_timeoutEnable(void)
{
    xTimerStart(display_timeout_tmr, 0);
}

static void display_timeoutDisable(void)
{
    xTimerStop(display_timeout_tmr, 0);
}

static void display_alertTimerEnable(void)
{
    xTimerStart(display_alert_tmr, 0);
}

static void display_alertTimerDisable(void)
{
    xTimerStop(display_alert_tmr, 0);
}

static void vDisplayUpdate(void)
{
    // system status updates
    display_updateSleepStatus();
    bool uiActive = (display_data.state != DISPLAY_STATE_GO_TO_SLEEP);

    SensorData_t display_sensor_data;
    if (xQueueReceive(display_sensor_info_queue, &display_sensor_data, 0))
    {
        display_updateBatteryInfo(&display_info, display_sensor_data);
    }

    // touchscreen
    uint32_t touchscreen_pressed = false;
    if (xTaskNotifyWait(0, 0, &touchscreen_pressed, 0))
    {
        CST816S_read_touch();
    }
    display_data.touch_active = CST816S_get_touch_active();

    bool alert_active = display_check_for_alerts();
    display_data.button_pressed = (xSemaphoreTake(button_semphr, 0) == pdTRUE);

    if (alert_active)
    {
        set_return_screen(display_data.screen);
        display_change_screen(DISPLAY_SCREEN_ALERT);
        display_alertTimerEnable();
    }
    else
    {
        // timeout timer refresh
        if (uiActive || display_data.touch_active || display_data.button_pressed)
        {
            display_timeoutRefresh();
            display_data.button_pressed = false;
        }
        else
        {
            display_data.state = DISPLAY_STATE_GO_TO_SLEEP;
        }

        if (display_data.button_pressed)
        {
            display_buttonHandler();
        }
    }
}


/** Public Functions **/

void Display_Task(void * arg)
{
    UNUSED_PARAMETER(arg);

    display_timeoutDisable();

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

    const TickType_t xFrequency = pdMS_TO_TICKS(20);

    while(1)
    {
        switch(display_data.state)
        {
            case DISPLAY_STATE_INITIALIZATION:

                // init touch input
                lv_indev_drv_init(&indev_drv);
                indev_drv.type = LV_INDEV_TYPE_POINTER;
                indev_drv.read_cb = touchscreen_read;
                lv_indev_drv_register(&indev_drv);

                // show boot up screen for brief period
                display_data.screen = DISPLAY_SCREEN_INITIALIZATION;
                display_change_screen(display_data.screen);
                disaplay_updateBrightness(BACKLIGHT_HIGH);

                vTaskDelay(pdMS_TO_TICKS(1000));
                display_data.screen = DISPLAY_SCREEN_HOME;
                display_change_screen(display_data.screen);
                // vTaskResume(thUIupdate);

                display_timeoutEnable();
                display_data.state = DISPLAY_STATE_RUN;
                vTaskDelay(xFrequency);
                break;

            case DISPLAY_STATE_RUN:
                vDisplayUpdate();

                if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE)
                {
                    lv_task_handler();
                    xSemaphoreGive(lvgl_mutex);
                }
                vTaskDelay(xFrequency);
                break;

            case DISPLAY_STATE_GO_TO_SLEEP:
                display_backlight_set(BACKLIGHT_OFF);
                display_off();
                display_go_to_sleep();
                display_data.active = false;
                display_data.state = DISPLAY_STATE_SLEEP;
                vTaskSuspend(xTaskGetCurrentTaskHandle());
                break;

            case DISPLAY_STATE_SLEEP:
                if (display_data.initialized)
                {
                    display_data.state = DISPLAY_STATE_RUN;
                    display_timeoutEnable();
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

    display_timeoutDisable();
    display_data.state = DISPLAY_STATE_GO_TO_SLEEP;
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
    return display_data.screen;
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

