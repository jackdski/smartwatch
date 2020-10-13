//
// Created by jack on 10/13/20.
//

#include "display_common.h"
#include "lvgl/lvgl.h"
#include "display_settings.h"

//extern lv_obj_t home_scr;
extern lv_obj_t * settings_scr;

//static void event_handler(lv_obj_t * obj, lv_event_t event);

#define NUM_SETTINGS				7
#define Y_OFFSET_LABEL_FROM_TITLE	55
#define Y_OFFSET_SWITCH_TO_SWITCH	35
#define X_OFFSET_LABEL_TO_BORDER	10
#define X_OFFSET_SWITCH_TO_BORDER	170

void settings_screen(void)
{
    display_battery_layer(settings_scr);
    display_screen_title(settings_scr, "Settings");

    /** SWITCHES **/

    // Switches text style
    static lv_style_t switches_style;
    lv_style_init(&switches_style);
    lv_style_set_text_font(&switches_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_14);

    lv_obj_t * labels[NUM_SETTINGS];
    lv_obj_t * selector[NUM_SETTINGS];
    char * label_strings[NUM_SETTINGS] = {
        "24-Hour",
        "Data Format",
        "Left-Hand",
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

//    void * event_handlers[NUM_SETTINGS] = {};

    uint8_t i;
    for(i = 0; i < NUM_SETTINGS; i++)
    {
        labels[i] = lv_label_create(settings_scr, NULL);
        lv_label_set_text(labels[i], label_strings[i]);
        lv_obj_add_style(labels[i], LV_LABEL_PART_MAIN, &switches_style);
        lv_obj_set_pos(labels[i],
                       X_OFFSET_LABEL_TO_BORDER,
                       (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i)));

        if(setting_type[i] == SETTING_SWITCH)
        {
            selector[i] = lv_switch_create(settings_scr, NULL);
            lv_obj_set_pos(selector[i],
                           X_OFFSET_SWITCH_TO_BORDER,
                           (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i)) - 5);
        }
        else if(setting_type[i] == SETTING_BUTTON)
        {
            selector[i] = lv_btn_create(settings_scr, NULL);
            lv_obj_t * lbl = lv_label_create(selector[i], NULL);
            lv_label_set_text(lbl, LV_SYMBOL_RIGHT);
            lv_obj_set_pos(selector[i],
                           X_OFFSET_SWITCH_TO_BORDER,
                           (Y_OFFSET_LABEL_FROM_TITLE + (Y_OFFSET_SWITCH_TO_SWITCH * i)) - 5);
        }
        //    lv_obj_set_event_cb(sw1, event_handlers[i]);
    }
}

