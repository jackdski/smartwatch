//
// Created by jack on 7/12/20.
//

#include "lvgl/lvgl.h"
#include "home.h"

//extern lv_obj_t * home_scr;
//extern lv_obj_t * settings_scr;
//extern lv_obj_t * brightness_scr;
//extern lv_obj_t * steps_scr;
//extern lv_obj_t * heart_rate_scr;
//extern lv_obj_t * stopwatch_scr;

#define NUM_HOME_BUTTONS	4


static void settings_event_handler(lv_obj_t * obj, lv_event_t event)
{
//    lv_scr_load(settings_scr);
}

static void steps_event_handler(lv_obj_t * obj, lv_event_t event)
{
//    lv_scr_load(steps_scr);
}

static void brightness_event_handler(lv_obj_t * obj, lv_event_t event)
{
//    lv_scr_load(brightness_scr);
}

static void heart_rate_event_handler(lv_obj_t * obj, lv_event_t event)
{
//    lv_scr_load(heart_rate_scr);
}

static void stopwatch_event_handler(lv_obj_t * obj, lv_event_t event)
{
//    lv_scr_load(stopwatch_scr);
}

void home_update_time(Time_t updated_time);


void home_screen(void)
{
    lv_obj_clean(lv_scr_act());

    // show the charging icon if charging
    lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,1}};
    lv_obj_t *tileview;
//    tileview = lv_tileview_create(home_scr, NULL);
    tileview = lv_tileview_create(lv_scr_act(), NULL);

    lv_tileview_set_valid_positions(tileview, valid_pos, 3);
    lv_tileview_set_edge_flash(tileview, false);

    lv_obj_t * tile1 = lv_obj_create(tileview, NULL);
    lv_obj_set_size(tile1, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(tileview, tile1);

    /*Tile1: Time and Date */
    lv_style_t time_style;
    lv_style_init(&time_style);
    lv_style_set_text_font(&time_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_30);

    // Style "Settings" text
    lv_style_t date_style;
    lv_style_init(&date_style);
    lv_style_set_text_font(&date_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_20);

    lv_obj_t * time_label = lv_label_create(tile1, NULL);
    lv_label_set_text(time_label, "16:53");
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -15);
    lv_obj_add_style(time_label, LV_LABEL_PART_MAIN, &time_style);


    lv_obj_t * date_label = lv_label_create(tile1, NULL);
    lv_label_set_text(date_label, "October 10, 2020");
    lv_obj_align(date_label, NULL, LV_ALIGN_CENTER, 0, 15);
    lv_obj_add_style(date_label, LV_LABEL_PART_MAIN, &date_style);


    /*Tile2: List*/
    lv_obj_t * list = lv_list_create(tileview, NULL);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(list, 0, LV_VER_RES);
    lv_list_set_scroll_propagation(list, true);
    lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_HIDE);

    lv_obj_t * btns[NUM_HOME_BUTTONS];
    void * btn_event_handlers[NUM_HOME_BUTTONS] = {
        settings_event_handler,
        steps_event_handler,
        heart_rate_event_handler,
        brightness_event_handler
//        stopwatch_event_handler
    };

    char * btn_descriptions[NUM_HOME_BUTTONS] = {
        LV_SYMBOL_SETTINGS "   Settings",
        LV_SYMBOL_GPS "   Steps",
        LV_SYMBOL_IMAGE "   Heart Rate",
        LV_SYMBOL_EYE_OPEN "   Brightness"
//        "    Stopwatch"
    };


    lv_style_t btns_style;
    lv_style_init(&btns_style);
    lv_style_set_text_font(&btns_style, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_14);
    lv_style_set_pad_left(&btns_style, LV_STATE_DEFAULT, 10);

    uint8_t i;
    for(i = 0; i < NUM_HOME_BUTTONS; i++)
    {
        btns[i] = lv_list_add_btn(list, NULL, btn_descriptions[i]);
        lv_obj_add_style(btns[i], LV_LABEL_PART_MAIN, &btns_style);
        lv_obj_set_event_cb(btns[i], btn_event_handlers[i]);
    }
}