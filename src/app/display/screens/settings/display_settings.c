//
// Created by jack on 10/13/20.
//

#include "display_common.h"
#include "lvgl/lvgl.h"
#include "display_settings.h"
#include "display.h"
#include "common.h"

#include "settings.h"

//--- DEFINES ---//

#define NUM_SETTINGS				7
#define Y_OFFSET_LABEL_FROM_TITLE	55
#define Y_OFFSET_SWITCH_TO_SWITCH	35
#define X_OFFSET_LABEL_TO_BORDER	10
#define X_OFFSET_SWITCH_TO_BORDER	170


//--- TYPEDEFs ---//

typedef struct
{
    DisplaySettingType_E    type;
    char                    * text;
    void                    (* eventHandler)(lv_obj_t * obj, lv_event_t event);
} DisplaySettingChannelData_t;


//--- PRIVATE FUNCTIONS ---//

static void military_time_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_MILITARY_TIME);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}

static void date_format_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_DATE_FORMAT);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}

static void right_hand_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_RIGHT_HAND);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}

static void show_texts_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_SHOW_TEXTS);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}

static void show_emails_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_SHOW_EMAILS);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}

static void show_calls_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_SHOW_CALLS);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}

static void show_news_event_handler(lv_obj_t * obj, lv_event_t event)
{
    UNUSED_PARAMETER(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        toggle_setting(SETTING_SHOW_NEWS);
        lv_sw_toggle(obj, LV_ANIM_ON);
    }
}


//--- PRIVATE DATA ---//

static const DisplaySettingChannelData_t settingsData[SETTINGS_COUNT] =
{
    [SETTING_MILITARY_TIME] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "24-Hour",
        .eventHandler = military_time_event_handler,
    },
    [SETTING_DATE_FORMAT] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "Date Format",
        .eventHandler = date_format_event_handler,
    },
    [SETTING_RIGHT_HAND] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "Right-Hand",
        .eventHandler = right_hand_event_handler,
    },
    [SETTING_SHOW_TEXTS] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "Show Texts",
        .eventHandler = show_texts_event_handler,
    },
    [SETTING_SHOW_EMAILS] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "Show Email",
        .eventHandler = show_emails_event_handler,
    },
    [SETTING_SHOW_CALLS] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "Show Calls",
        .eventHandler = show_calls_event_handler,
    },
    [SETTING_SHOW_NEWS] =
    {
        .type   = SETTING_TYPE_SWITCH,
        .text   = "Show News",
        .eventHandler = show_news_event_handler,
    },
};

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


    lv_obj_t * labels[SETTINGS_COUNT];
    lv_obj_t * selector[SETTINGS_COUNT];

    for(Settings_E i = (Settings_E)0U; i < SETTINGS_COUNT; i++)
    {
        const lv_coord_t yPos = (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i));
        const lv_coord_t yPos_sw_btn = (yPos - 5);

        labels[i] = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_style(labels[i], LV_LABEL_STYLE_MAIN, &switches_style);
        lv_label_set_text(labels[i], settingsData[i].text);
        lv_obj_set_pos(labels[i],
                       X_OFFSET_LABEL_TO_BORDER,
                       yPos);

        if (settingsData[i].type == SETTING_TYPE_SWITCH)
        {
            selector[i] = lv_sw_create(lv_scr_act(), NULL);


            lv_obj_set_pos(selector[i],
                           X_OFFSET_SWITCH_TO_BORDER,
                           yPos_sw_btn);
        }
        else if (settingsData[i].type == SETTING_TYPE_BUTTON)
        {
            selector[i] = lv_btn_create(lv_scr_act(), NULL);
            lv_obj_t * lbl = lv_label_create(selector[i], NULL);
            lv_label_set_text(lbl, LV_SYMBOL_RIGHT);
            lv_obj_set_pos(selector[i],
                           X_OFFSET_SWITCH_TO_BORDER,
                           yPos_sw_btn);
        }
        else
        {
            // type unsupported
        }

        lv_obj_set_event_cb(labels[i], settingsData[i].eventHandler);
    }
}

