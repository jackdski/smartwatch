//
// Created by jack on 7/12/20.
//

#include "lvgl/lvgl.h"
#include "display_home.h"

#include "display.h"
#include "components/time/time.h"
#include "app_settings.h"

#define NUM_HOME_BUTTONS	4

static lv_obj_t * time_label;
static lv_obj_t * date_label;

static void time_update_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        char time_str[10];
        if(is_military_time())
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

static void settings_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    UNUSED_PARAMETER(event);
    display_change_screen(DISPLAY_SCREEN_SETTINGS);
}

static void brightness_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    UNUSED_PARAMETER(event);
    display_change_screen(DISPLAY_SCREEN_BRIGHTNESS);
}

static void steps_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    UNUSED_PARAMETER(event);
    display_change_screen(DISPLAY_SCREEN_STEPS);
}

static void heart_rate_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    UNUSED_PARAMETER(event);
    display_change_screen(DISPLAY_SCREEN_HEART_RATE);
}

static void stopwatch_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    UNUSED_PARAMETER(event);
    display_change_screen(DISPLAY_SCREEN_SETTINGS);
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

    static lv_style_t btns_style;
    lv_style_copy(&btns_style, &lv_style_plain);
    btns_style.text.font = &lv_font_roboto_16;
    btns_style.body.padding.left = 10;

    // Tileview
    lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,1}};
    lv_obj_t * tileview;
    tileview = lv_tileview_create(lv_scr_act(), NULL);

    lv_obj_t * tile1 = lv_obj_create(tileview, NULL);
    time_label = lv_label_create(tile1, NULL);
    lv_label_set_style(time_label, LV_LABEL_STYLE_MAIN, &time_style);

    date_label = lv_label_create(tile1, NULL);
    lv_label_set_style(date_label, LV_LABEL_STYLE_MAIN, &date_style);

    lv_obj_t * list = lv_list_create(tileview, NULL);

    // get time: "15:37" or "3:37pm"
    char time_str[10];
    if(is_military_time())
    {
        format_time_military(time_str);
    }
    else
    {
        format_time_standard(time_str);
    }

    // get date: "Monday October 15, 2021"
    char date_str[30];
    format_date(date_str);

    // show the charging icon if charging
    lv_tileview_set_valid_positions(tileview, valid_pos, 3);
    lv_tileview_set_edge_flash(tileview, false);

    lv_obj_set_size(tile1, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(tileview, tile1);

    // Set time text
    lv_label_set_text(time_label, time_str);
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_event_cb(time_label, time_update_event_handler);

    // Set date text
    lv_label_set_text(date_label, date_str);
    lv_obj_align(date_label, NULL, LV_ALIGN_CENTER, 0, 15);

    /*Tile2: List*/
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(list, 0, LV_VER_RES);
    lv_list_set_scroll_propagation(list, true);
//    lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_HIDE);

    // Buttons
    lv_obj_t * settings_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_style(settings_btn, LV_BTN_STYLE_REL, &btns_style);
    settings_btn = lv_list_add_btn(list, NULL, LV_SYMBOL_SETTINGS "   Settings");
    lv_obj_set_event_cb(settings_btn, settings_event_handler);

    lv_obj_t * steps_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_style(steps_btn, LV_BTN_STYLE_REL, &btns_style);
    steps_btn = lv_list_add_btn(list, NULL, LV_SYMBOL_GPS "   Steps");
    lv_obj_set_event_cb(steps_btn, steps_event_handler);

    lv_obj_t * heart_rate_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_style(heart_rate_btn, LV_BTN_STYLE_REL, &btns_style);
    heart_rate_btn = lv_list_add_btn(list, NULL, LV_SYMBOL_IMAGE "   Heart Rate");
    lv_obj_set_event_cb(heart_rate_btn, heart_rate_event_handler);

    lv_obj_t * brightness_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_style(brightness_btn, LV_BTN_STYLE_REL, &btns_style);
    brightness_btn = lv_list_add_btn(list, NULL, LV_SYMBOL_EYE_OPEN "   Brightness");
    lv_obj_set_event_cb(brightness_btn, brightness_event_handler);

    lv_obj_t * stopwatch_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_style(stopwatch_btn, LV_BTN_STYLE_REL, &btns_style);
    stopwatch_btn = lv_list_add_btn(list, NULL, "    Stopwatch");
    lv_obj_set_event_cb(stopwatch_btn, stopwatch_event_handler);
}