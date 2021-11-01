//
// Created by jack on 10/13/20.
//

#include "display.h"
#include "display_common.h"
#include "lvgl/lvgl.h"

// FreeRTOS files
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Screens
#include "display_alerts.h"
#include "display_boot_up.h"
#include "display_home.h"
#include "display_common.h"
#include "display_brightness.h"
#include "display_heart_rate.h"
#include "display_settings.h"
#include "display_steps.h"


// Public Variables
extern SemaphoreHandle_t lvgl_mutex;


// Private Variables
lv_obj_t * battery_info_label;
lv_obj_t * charging_info_label;


/** PRIVATE FUNCTIONS  **/

static const char * get_battery_status_icon(DisplayBatteryStatus_E battery_status)
{
    switch(battery_status)
    {
    case BATTERY_FULL:
        return LV_SYMBOL_BATTERY_FULL;
    
    case BATTERY_ALMOST_FULL:
        return LV_SYMBOL_BATTERY_3;
    
    case BATTERY_HALF_FULL:
        return LV_SYMBOL_BATTERY_2;
    
    case BATTERY_LOW:
        return LV_SYMBOL_BATTERY_1;
    
    case BATTERY_EMPTY:
    default:
        return LV_SYMBOL_BATTERY_EMPTY;
    }
}


/** PUBLIC FUNCTIONS  **/

DisplayScreens_E display_change_screen(DisplayScreens_E screen)
{    
    DisplayScreens_E new_screen = display_get_current_screen();
    
    if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)))
    {
        new_screen = screen;

        switch(screen) {
        case DISPLAY_SCREEN_INITIALIZATION:     display_boot_up(); break;
        case DISPLAY_SCREEN_ALERT:              display_alert(); break;
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
    return new_screen;
}

DisplayScreens_E display_get_parent_screen(DisplayScreens_E screen)
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

void display_battery_layer(lv_obj_t * parent)
{
    bool charging_status = display_get_charging_status();
    DisplayBatteryStatus_E battery_status = display_get_battery_status();

    battery_info_label = lv_label_create(parent, NULL);
    lv_label_set_text(battery_info_label, get_battery_status_icon(battery_status));
    lv_obj_set_pos(battery_info_label, 215, 6);

    if(charging_status == true)
    {
        charging_info_label = lv_label_create(parent, NULL);
        lv_label_set_text(charging_info_label, LV_SYMBOL_CHARGE);
        lv_obj_set_pos(charging_info_label, 195, 6);
    }

    // TODO: LV_SYMBOL_BLUETOOTH
}

// Title Functions
void display_screen_title(lv_obj_t * parent, char * title)
{
    // Label
    static lv_style_t label_style;
    lv_style_copy(&label_style, &lv_style_plain);
    label_style.text.font = &lv_font_roboto_22;

    lv_obj_t * label = lv_label_create(parent, NULL);
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style);

    lv_label_set_text(label, title);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 10);

    // Line
    static lv_style_t line_style;
    lv_style_copy(&line_style, &lv_style_plain);
    line_style.line.width = 1;
    line_style.line.color = LV_COLOR_WHITE;
    line_style.line.rounded = 1;

    static lv_point_t line_points[] = { {5, 25}, {LV_HOR_RES_MAX - 5, 25} };

    /* Create a line */
    lv_obj_t * line1;
    line1 = lv_line_create(parent, NULL);
    lv_line_set_style(line1, LV_LINE_STYLE_MAIN, &line_style);
    lv_line_set_points(line1, line_points, 2);
    lv_obj_set_pos(line1, 0, 15);
}
