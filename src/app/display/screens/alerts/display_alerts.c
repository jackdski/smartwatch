#include "display_alerts.h"
#include "alerts.h"

#include "display.h"
#include "lvgl/lvgl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

extern QueueHandle_t alerts_queue;
extern TimerHandle_t display_alert_tmr;

Alerts_E alert = NO_ACTIVE_ALERT;
DisplayScreens_E return_screen = DISPLAY_SCREEN_HOME;


bool display_check_for_alerts(void)
{   
    Alerts_E new_alert = NO_ACTIVE_ALERT;
    if(xQueueReceive(alerts_queue, &new_alert, 0))
    {
        alert = new_alert;
    }

    return (alert != NO_ACTIVE_ALERT);
}

void set_return_screen(DisplayScreens_E screen)
{
    return_screen = screen;
}

void display_alert(void)
{
    lv_obj_clean(lv_scr_act());
    lv_obj_t * alerts_scr = lv_scr_act();

    lv_obj_t * alert_label;
    
    // Style bpm text
    static lv_style_t alert_label_style;
    lv_style_copy(&alert_label_style, &lv_style_plain);
    alert_label_style.text.font = &lv_font_roboto_22;


    alert_label = lv_label_create(alerts_scr, NULL);
    lv_label_set_style(alerts_scr, LV_LABEL_STYLE_MAIN, &alert_label_style);

    switch(alert)
    {
        case ALERT_BATTERY_LOW:
            lv_label_set_text(alert_label, "Low Battery!");
            break;

        case ALERT_BATTERY_CHARGED:
            lv_label_set_text(alert_label, "Battery Fully Charged!");
            break;

        case ALERT_HEART_RATE_HIGH:
            lv_label_set_text(alert_label, "High Heart Rate Detected");
            break;

        case ALERT_HEART_RATE_LOW:
            lv_label_set_text(alert_label, "Low Heart Rate Detected");
            break;

        case NO_ACTIVE_ALERT:
        default:
            break;
    }

    lv_obj_set_pos(alert_label, (LV_HOR_RES_MAX / 2), 100);
}


void display_alert_callback(TimerHandle_t xTimer)
{
    UNUSED_PARAMETER(xTimer);

    alert = NO_ACTIVE_ALERT;
    return_screen = DISPLAY_SCREEN_HOME;
    // display_alert_timer_disable(); // TODO
}