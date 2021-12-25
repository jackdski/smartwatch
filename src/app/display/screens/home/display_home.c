//
// Created by jack on 7/12/20.
//

#include "lvgl/lvgl.h"
#include "display_home.h"

#include "display.h"
#include "components/time/time.h"

#define NUM_HOME_BUTTONS	4

static lv_obj_t * time_label;
static lv_obj_t * date_label;

static void time_update_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        char time_str[10];
        if(get_setting(SETTING_MILITARY_TIME) == true)
        {
            format_time_military(time_str);
        }
        else
        {
            format_time_standard(time_str);
        }
        lv_label_set_text(obj, time_str);
    }
}

void home_screen(void)
{
    lv_obj_clean(lv_scr_act());

    /*Tile1: Time and Date */
    static lv_style_t time_style;
    lv_style_copy(&time_style, &lv_style_plain);
    time_style.text.font = &lv_font_roboto_22;

    // Style "Settings" text
    static lv_style_t date_style;
    lv_style_copy(&date_style, &lv_style_plain);
    date_style.text.font = &lv_font_roboto_28;

    lv_obj_t * tile1 = lv_obj_create(lv_scr_act(), NULL);
    time_label = lv_label_create(tile1, NULL);
    lv_label_set_style(time_label, LV_LABEL_STYLE_MAIN, &time_style);

    date_label = lv_label_create(tile1, NULL);
    lv_label_set_style(date_label, LV_LABEL_STYLE_MAIN, &date_style);


    // get time: "15:37" or "3:37pm"
    char time_str[10];
    if(get_setting(SETTING_MILITARY_TIME) == true)
    {
        format_time_military(time_str);
    }
    else
    {
        format_time_standard(time_str);
    }

    // get date: format: "Weekday Month Day, Year"
    char date_str[30];
    format_date(date_str);

    // show the charging icon if charging
    lv_obj_set_size(tile1, LV_HOR_RES, LV_VER_RES);
    // Set time text
    lv_label_set_text(time_label, time_str);
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_event_cb(time_label, time_update_event_handler);

    // Set date text
    lv_label_set_text(date_label, date_str);
    lv_obj_align(date_label, NULL, LV_ALIGN_CENTER, 0, 15);
}