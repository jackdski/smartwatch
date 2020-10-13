//
// Created by jack on 7/12/20.
//

#include "display_boot_up.h"
#include "display.h"


void display_boot_up(lv_obj_t * parent) {
    /* Create the main label */
//    lv_obj_t * main_label = lv_label_create(parent, NULL);
//
//    /* Position the main label */
//    lv_label_set_align(main_label, LV_LABEL_ALIGN_CENTER);
//    lv_label_set_text(main_label, "JD");
//    lv_obj_set_width(main_label, 200);
//    lv_obj_align(main_label, parent, LV_ALIGN_CENTER, 0, -120);

    lv_obj_t * text;
    lv_style_t label_style;

    text = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_size(text, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    lv_obj_align(text, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_style_init(&label_style);
    lv_style_set_radius(&label_style, LV_STATE_DEFAULT, 10);
    lv_style_set_bg_opa(&label_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&label_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_color(&label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    lv_textarea_set_text(text, "JD Smartwatch");    /*Set an initial text*/
}
