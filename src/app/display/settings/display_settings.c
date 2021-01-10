//
// Created by jack on 10/13/20.
//

#include "display_common.h"
#include "lvgl/lvgl.h"
#include "display_settings.h"
#include "display.h"
#include "common.h"


#define NUM_SETTINGS				7
#define Y_OFFSET_LABEL_FROM_TITLE	55
#define Y_OFFSET_SWITCH_TO_SWITCH	35
#define X_OFFSET_LABEL_TO_BORDER	10
#define X_OFFSET_SWITCH_TO_BORDER	170


static void military_time_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_MILITARY_TIME);
    }
}

static void date_format_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_DATE_FORMAT);
    }
}

static void right_hand_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_RIGHT_HAND);
    }
}

static void show_texts_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_SHOW_TEXTS);
    }
}

static void show_emails_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_SHOW_EMAILS);
    }
}

static void show_calls_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_SHOW_CALLS);
    }
}

static void show_news_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        display_setting_changed(SETTING_SHOW_NEWS);
    }
}


void display_settings_screen(void)
{
    lv_obj_clean(lv_scr_act());

    display_battery_layer(lv_scr_act());
    display_screen_title(lv_scr_act(), "Settings");

    /** SWITCHES **/

    // Switches text style
    static lv_style_t switches_style;
    lv_style_copy(&switches_style, &lv_style_plain);
//    lv_style_set_text_font(&switches_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_14);
    switches_style.text.font = &lv_font_roboto_12;


    lv_obj_t * labels[NUM_SETTINGS];
    lv_obj_t * selector[NUM_SETTINGS];
    char * label_strings[NUM_SETTINGS] = {
        "24-Hour",
        "Date Format",
        "Right-Hand",
        "Show Texts",
        "Show Calls",
        "Show Email",
        "Show News"
    };

    eDisplaySettingSelector setting_type[NUM_SETTINGS] = {
        SETTING_SWITCH,
        SETTING_SWITCH,
        SETTING_SWITCH,
        SETTING_SWITCH,
        SETTING_SWITCH,
        SETTING_SWITCH,
        SETTING_SWITCH
    };

    void * event_handlers[NUM_SETTINGS] = {
        military_time_event_handler,
        date_format_event_handler,
        right_hand_event_handler,
        show_texts_event_handler,
        show_emails_event_handler,
        show_calls_event_handler,
        show_news_event_handler
    };

    uint8_t i;
    for(i = 0; i < NUM_SETTINGS; i++)
    {
        labels[i] = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_style(labels[i], LV_LABEL_STYLE_MAIN, &switches_style);
        lv_label_set_text(labels[i], label_strings[i]);
        lv_obj_set_pos(labels[i],
                       X_OFFSET_LABEL_TO_BORDER,
                       (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i)));

        if(setting_type[i] == SETTING_SWITCH)
        {
            selector[i] = lv_sw_create(lv_scr_act(), NULL);
            lv_obj_set_pos(selector[i],
                           X_OFFSET_SWITCH_TO_BORDER,
                           (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i)) - 5);
        }
        else if(setting_type[i] == SETTING_BUTTON)
        {
            selector[i] = lv_btn_create(lv_scr_act(), NULL);
            lv_obj_t * lbl = lv_label_create(selector[i], NULL);
            lv_label_set_text(lbl, LV_SYMBOL_RIGHT);
            lv_obj_set_pos(selector[i],
                           X_OFFSET_SWITCH_TO_BORDER,
                           (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i)) - 5);
        }
        lv_obj_set_event_cb(labels[i], event_handlers[i]);
    }
}

